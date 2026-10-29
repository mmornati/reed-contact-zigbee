# 3. Firmware: compilazione e flash

🌐 [English](../03-firmware.md) · [Français](../fr/03-firmware.md) · **Italiano**

Sketch: [`firmware/reed_contact/reed_contact.ino`](../../firmware/reed_contact/reed_contact.ino)

> **Due firmware a scelta, lo stesso hardware.** Questa guida descrive il firmware Arduino (opzioni A e B). Se gestisci già i tuoi dispositivi con ESPHome, puoi flashare [`esphome/reed-contact.yaml`](../../esphome/reed-contact.yaml) al suo posto: la sezione *Opzione C: ESPHome* più sotto spiega le differenze e come flasharlo. Il cablaggio è identico in entrambi i casi.

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

## Opzione C: ESPHome

Se gestisci già i tuoi dispositivi con [ESPHome](https://esphome.io/), puoi flashare [`esphome/reed-contact.yaml`](../../esphome/reed-contact.yaml) al posto dello sketch. Usa il supporto Zigbee nativo di ESPHome per l'ESP32-C6, quindi resta un router Zigbee, senza WiFi. **Il cablaggio è esattamente lo stesso**: reed su D2 / GPIO2 verso GND, antenna esterna tramite GPIO3 / GPIO14, pulsante BOOT su GPIO9, LED su GPIO15.

### Firmware Arduino o ESPHome?

| | Firmware Arduino (opzioni A, B) | ESPHome (opzione C) |
|---|---|---|
| Cluster Zigbee | Contatto **IAS Zone**, lo standard per i sensori porta/finestra | **Binary Input**: ESPHome non supporta ancora IAS Zone |
| Converter di Zigbee2MQTT | [`reed_contact.mjs`](../../zigbee2mqtt/reed_contact.mjs) | [`reed_contact_esphome.mjs`](../../zigbee2mqtt/reed_contact_esphome.mjs) |
| Risultato in Z2M e Home Assistant | `contact`, `binary_sensor` con device class *door* | lo stesso |
| ZHA e altri coordinatori | Riconosciuto come sensore di contatto | Visto come un ingresso binario generico |
| Stringa del modello | `C6ReedContact` | `C6ReedContactESPHome` |
| Flasher dal browser e binari delle release | Sì | No, lo compili con ESPHome |
| Aggiornamenti | USB | USB. Via Zigbee solo con la versione OTA sperimentale (vedi sotto) |
| Versione di ESPHome | | ramo `dev` (2026.11 o successiva) |
| Personalizzazione | Modifichi lo sketch C++ | Modifichi lo YAML: un secondo reed, un relè o una sonda di temperatura sono poche righe |

Scegli il firmware Arduino se vuoi il sensore di contatto standard e il flash con un clic. Scegli ESPHome se preferisci lo YAML e hai già un flusso di lavoro con ESPHome.

### Cosa fa lo YAML

Tutto quello che fa lo sketch, tranne l'enrollment IAS Zone, che a una Binary Input non serve:

- **Router** Zigbee, alimentato da rete, produttore `DIY`, modello `C6ReedContactESPHome`.
- All'avvio passa sull'antenna esterna U.FL.
- Legge il reed su **GPIO2** con il pull-up interno e un debounce di **200 ms**. `presentValue` è `true` quando il reed è aperto (magnete lontano o filo tagliato) e `false` quando è chiuso, quindi un guasto si legge sempre come *aperto*.
- Rimanda lo stato **ogni 5 minuti** come heartbeat.
- **LED:** lampeggio lento mentre cerca una rete, spento una volta collegato.
- **Reset di fabbrica:** tieni premuto **BOOT** per 3 s, **poi rilascialo**. La scheda lascia la rete e si riavvia pronta per l'abbinamento. Il firmware Arduino si resetta mentre tieni ancora premuto il pulsante; ESPHome aspetta il rilascio.

### Compilazione e flash

1. Installa ESPHome dal ramo `dev`. Lo Zigbee nativo e l'opzione `manufacturer` non sono ancora in una versione stabile:

   ```bash
   uv tool install https://github.com/esphome/esphome/archive/dev.zip
   ```

   Oppure `pip install https://github.com/esphome/esphome/archive/dev.zip` in un virtualenv, o l'immagine Docker `ghcr.io/esphome/esphome:dev`.
2. Collega lo XIAO via USB-C (**senza** 12 V sul buck). Se prima aveva il firmware Arduino, cancellalo per non lasciare vecchi dati della rete Zigbee:

   ```bash
   esptool --chip esp32c6 erase-flash
   ```

3. Compila e carica (la prima esecuzione scarica la toolchain ESP-IDF):

   ```bash
   esphome run esphome/reed-contact.yaml
   ```

   Scegli la porta seriale USB quando richiesto. Se la porta non viene trovata: tieni premuto **BOOT**, premi **RESET**, rilascia **BOOT**, poi riprova.
4. Segui i log con `esphome logs esphome/reed-contact.yaml`.
5. Installa [`zigbee2mqtt/reed_contact_esphome.mjs`](../../zigbee2mqtt/reed_contact_esphome.mjs) come converter di Z2M, poi abbina il sensore come descritto nella [guida successiva](04-zigbee2mqtt-and-home-assistant.md).

### Impostazioni che potresti voler cambiare

| Define Arduino | Nello YAML |
|---|---|
| `REED_PIN` | `binary_sensor` → `contact` → `pin` → `number` |
| `DEBOUNCE_MS` | `filters` → `delayed_on_off` |
| `HEARTBEAT_MS` | la voce `5min` sotto `interval` |
| `FACTORY_RESET_HOLD_MS` | `on_click` → `min_length` |
| `useExternalAntenna()` | il blocco `on_boot` (toglilo per usare l'antenna ceramica) |

### Aggiornare una versione ESPHome

L'OTA classico di ESPHome passa dal WiFi, che questa versione non usa. Scollega i 12 V, collega l'USB e rilancia `esphome run` **senza cancellare**: la scheda conserva le chiavi di rete e si ricollega senza un nuovo abbinamento.

Gli aggiornamenti via Zigbee sono possibili con un componente sperimentale `ota: platform: zigbee`, in revisione in [mmornati/esphome#1](https://github.com/mmornati/esphome/pull/1) e non ancora parte di ESPHome. Con questo, Zigbee2MQTT invia il nuovo firmware attraverso la mesh. È lento (circa 30 minuti per 360 KB), tienine conto.

### Passare da una versione all'altra

La stringa del modello cambia, quindi Z2M vede un nuovo dispositivo. Rimuovilo in Z2M, flasha l'altra versione con una cancellazione completa, abbinalo di nuovo e dagli lo stesso nome (`garage_door_sensor`). Home Assistant mantiene gli stessi ID delle entità.
