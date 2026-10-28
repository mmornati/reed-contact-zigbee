# 4. Zigbee2MQTT e Home Assistant

🌐 [English](../04-zigbee2mqtt-and-home-assistant.md) · [Français](../fr/04-zigbee2mqtt-and-home-assistant.md) · **Italiano**

## 4.1 Installa l'external converter (Zigbee2MQTT 2.x)

Z2M non conosce un dispositivo fai-da-te, quindi un piccolo converter gli dice "questo è un sensore di contatto".

1. Copia [`zigbee2mqtt/reed_contact.mjs`](../../zigbee2mqtt/reed_contact.mjs) nella cartella dati di Z2M, dentro `external_converters/`:
   - Add-on di HA: `/config/zigbee2mqtt/external_converters/reed_contact.mjs` (usa l'add-on File editor o Samba)
   - Docker: `<your data volume>/external_converters/reed_contact.mjs`
2. Riavvia Zigbee2MQTT. In Z2M 2.x tutto ciò che sta in `external_converters/` viene caricato automaticamente, senza modificare `configuration.yaml`.
3. Controlla nel log di Z2M che ci sia una riga che dice che l'external converter `reed_contact.mjs` è stato caricato.

## 4.2 Abbina il sensore

1. Nell'interfaccia di Z2M, clicca **Permit join (All)** (consenti l'accesso a tutti). Scegliere il coordinatore o un router vicino al garage aiuta.
2. Alimenta lo XIAO (USB al banco, o 12 V una volta installato). Il LED lampeggia lentamente, poi si spegne una volta collegato.
3. Z2M lo interroga (interview) e dovrebbe mostrare **DIY C6ReedContact, "Mains-powered reed contact sensor and Zigbee router (XIAO ESP32-C6)"**, tipo **Router**.
4. Rinominalo in **`garage_door_sensor`** e spunta *Update Home Assistant entity ID*.
5. Controlla la scheda *Exposes*: `contact` deve essere **true** con il magnete davanti e **false** senza.

Se compare come "Unsupported", il converter non è caricato o la stringa del modello non corrisponde. Vedi [Risoluzione dei problemi](05-troubleshooting.md).

### Impostazioni di Z2M consigliate

- *Settings → Availability* attivo. I router vengono interrogati attivamente, quindi HA vede `unavailable` se il sensore muore.
- Lo ZB-SW01 va chiamato **`garage_opener`**, il che dà `switch.garage_opener`.

## 4.3 Entità in Home Assistant

Tramite MQTT discovery ottieni:

| Entità | Significato |
|---|---|
| `binary_sensor.garage_door_sensor_contact` | device class *door*: **on = non completamente chiusa**, **off = chiusa** |
| `sensor.garage_door_sensor_linkquality` | LQI Zigbee (disabilitata di default) |
| `switch.garage_opener` | Relè ZB-SW01 |

> Controlla gli entity ID reali in *Settings → Devices → garage_door_sensor*. Se sono diversi, cercali e sostituiscili nel package qui sotto.

## 4.4 Installa il package di HA

[`homeassistant/garage_door.yaml`](../../homeassistant/garage_door.yaml) contiene:

| Elemento | Cosa fa |
|---|---|
| `script.garage_door_pulse` | Manda un impulso di 800 ms allo ZB-SW01, come premere il pulsante a muro |
| `script.garage_door_open` | Manda l'impulso solo se la porta è chiusa |
| `script.garage_door_close` | Manda l'impulso solo se la porta è aperta, poi **controlla che sia completamente chiusa entro 90 s** e ti avvisa se non lo è |
| `cover.garage_door` | Template cover (device class *garage*) con pulsanti apri/chiudi e lo stato del sensore |
| `input_number.garage_door_open_alert_minutes` | Ritardo prima dell'avviso "lasciata aperta" (predefinito 10 min) |
| Automazione *left open* | Avvisa quando la porta non è completamente chiusa da N minuti |
| Automazione *night check* | Avvisa alle 22:00 se la porta non è chiusa |
| Automazione *sensor offline* | Avvisa se il sensore è non disponibile da 15 min |

Passi:

1. In `configuration.yaml`, se non c'è già:

   ```yaml
   homeassistant:
     packages: !include_dir_named packages
   ```

2. Copia il file in `/config/packages/garage_door.yaml`.
3. Sostituisci `notify.notify` con il tuo servizio di notifica, es. `notify.mobile_app_<your_phone>` o il tuo notifier ntfy.
4. *Developer tools → YAML → Check configuration*, poi **Restart**. Le template cover e i nuovi package richiedono un riavvio, non basta un reload.
5. Aggiungi `cover.garage_door` a una dashboard. Una scheda *Tile* con la funzione apri/chiudi della cover funziona bene.

### Perché la cover non ha lo "stop"

Lo ZB-SW01 ha un solo pulsante: impulso = apri / ferma / chiudi, a seconda del motore. Con un solo sensore di "chiuso", HA non può sapere se la porta si sta muovendo, quindi un pulsante di stop si comporterebbe in modo imprevedibile. Gli script apri/chiudi mandano l'impulso solo quando è sicuro: apre solo se è chiusa, chiude solo se non è chiusa.

### Il controllo "non completamente chiusa"

È l'obiettivo principale del progetto. È coperto in tre modi:

1. **Completamente chiusa** è l'unico stato che risulta `off`. Qualsiasi apertura di qualche centimetro risulta `on` (non chiusa).
2. Dopo una chiusura richiesta da HA, `script.garage_door_close` aspetta 90 s e ti avvisa se la porta non è ancora chiusa. Così intercetti un ostacolo, l'inversione di sicurezza del motore o una porta fermatasi a metà.
3. Qualunque cosa abbia aperto la porta (tastierino, telecomando, pulsante a muro, HA), l'automazione *left open* ti avvisa dopo N minuti, e il controllo delle 22:00 è un ultimo promemoria.

## 4.5 Idee per il futuro

- Una **notifica interattiva** sull'avviso "lasciata aperta" con un pulsante *Chiudi ora* che chiama `script.garage_door_close`.
- Chiusura automatica di notte, solo se un sensore di movimento dice che in garage non c'è nessuno.
- Un secondo reed in posizione di apertura completa (ce n'è uno di scorta nel kit) per avere gli stati aperta / chiusa / in apertura / in chiusura. Serve un secondo endpoint nel firmware.
