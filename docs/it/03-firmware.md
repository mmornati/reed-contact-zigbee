# 3. Firmware: compilazione e flash

🌐 [English](../03-firmware.md) · [Français](../fr/03-firmware.md) · **Italiano**

Sketch: [`firmware/reed_contact/reed_contact.ino`](../../firmware/reed_contact/reed_contact.ino)

## Cosa fa

- Si avvia come **router Zigbee**, quindi resta sempre acceso e inoltra il traffico degli altri dispositivi.
- Passa la radio sull'**antenna esterna U.FL** (GPIO3 = LOW, GPIO14 = HIGH).
- Espone un endpoint **contatto IAS Zone** (produttore `DIY`, modello `C6ReedContact`).
- Legge il reed su **D2 / GPIO2**: `LOW` = completamente chiuso, `HIGH` = non chiuso. Con un debounce di 200 ms.
- Invia lo stato a ogni cambiamento, e di nuovo **ogni 5 minuti** come heartbeat.
- Gestisce l'enrollment IAS Zone. Ricorda l'enrollment anche dopo un riavvio e riprova ogni minuto se il coordinatore non ha risposto.
- **LED:** lampeggio lento = cerca una rete, spento = collegato. Mentre tieni premuto BOOT lampeggia veloce.
- **Reset di fabbrica:** tieni premuto **BOOT** per 3 s. La scheda lascia la rete, dimentica l'enrollment e si riavvia pronta per l'abbinamento.

## Opzione A: PlatformIO (consigliato, nativo su Apple Silicon)

[`firmware/platformio.ini`](../../firmware/platformio.ini) contiene tutte le impostazioni di build (scheda, modalità router Zigbee, tabella delle partizioni), quindi non c'è niente da scegliere nei menu. Usa la piattaforma della community [pioarduino](https://github.com/pioarduino/platform-espressif32), che include il core arduino-esp32 3.x. La piattaforma ufficiale di PlatformIO si ferma al core 2.x, che non ha lo Zigbee.

1. Installa PlatformIO Core. Richiede Python 3.10–3.14; il `platformio` di Homebrew gira su un Python più recente che pioarduino rifiuta, quindi usa uv:

   ```bash
   uv tool install --python 3.13 platformio
   ```

   Oppure installa l'estensione *PlatformIO IDE* in VS Code, che si porta dietro il suo Python.
2. Compila (la prima volta scarica la piattaforma ESP32 e la toolchain, qualche centinaio di MB):

   ```bash
   cd firmware
   pio run
   ```

3. Collega lo XIAO via USB-C (con i 12 V sul buck **scollegati**). Per il primissimo flash cancella la memoria, poi carica:

   ```bash
   pio run -t erase
   pio run -t upload
   ```

   Se la porta non viene trovata: tieni premuto **BOOT**, premi brevemente **RESET**, rilascia **BOOT**, poi riprova il caricamento.
4. Guarda i log:

   ```bash
   pio device monitor
   ```

## Opzione B: Arduino IDE

> Su un Mac Apple Silicon, l'Arduino IDE ha bisogno di Rosetta: il suo strumento di preprocessing ctags è solo per Intel e fallisce con *"bad CPU type in executable"*. Installala con `softwareupdate --install-rosetta --agree-to-license`, oppure usa PlatformIO qui sopra.

1. Installa **Arduino IDE 2.x**.
2. *File → Preferences → Additional boards manager URLs* (URL aggiuntive per il gestore schede):
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. *Boards Manager* → installa **esp32 by Espressif Systems**, versione **3.3.0 o successiva**.
4. Apri `firmware/reed_contact/reed_contact.ino`.
5. Menu *Tools*:

| Impostazione | Valore |
|---|---|
| Board | **XIAO_ESP32C6** |
| Zigbee mode | **Zigbee ZCZR (coordinator/router)** |
| Partition Scheme | **Zigbee ZCZR 4MB with spiffs** |
| Erase All Flash Before Sketch Upload | **Enabled** per il primo flash, poi Disabled |
| USB CDC On Boot | Enabled (così vedi i log) |

Se scegli la Zigbee mode sbagliata, lo sketch si rifiuta di compilare con un messaggio chiaro.

Poi flasha:

1. Collega lo XIAO via USB-C (con i 12 V sul buck **scollegati**).
2. Clicca **Upload**. Se la porta non compare: tieni premuto **BOOT**, premi brevemente **RESET**, rilascia **BOOT**, poi riprova il caricamento.
3. Apri il Serial Monitor a 115200 baud.

## Primo avvio

Qualunque strumento tu abbia usato, il log seriale dovrebbe mostrare:

```
Starting Zigbee (router)
Joining network.......
```

Ora abbinalo in Zigbee2MQTT. Vedi [la guida successiva](04-zigbee2mqtt-and-home-assistant.md).

## Impostazioni che potresti voler cambiare

In cima allo sketch:

| Define | Predefinito | Significato |
|---|---|---|
| `REED_PIN` | `2` (D2) | Pin di ingresso del reed |
| `DEBOUNCE_MS` | `200` | Per quanto tempo il reed deve restare stabile |
| `HEARTBEAT_MS` | 5 min | Reinvio periodico dello stato |
| `FACTORY_RESET_HOLD_MS` | `3000` | Tempo di pressione di BOOT per il reset |

Per usare invece l'antenna ceramica integrata, togli la chiamata a `useExternalAntenna()`.

## Aggiornamenti successivi

Stacca i 12 V, collega l'USB e carica **senza cancellare**: un semplice `pio run -t upload`, oppure nell'Arduino IDE con *Erase All Flash = Disabled*. La scheda conserva le chiavi di rete e l'enrollment, e si ricollega da sola senza doverla riabbinare.
