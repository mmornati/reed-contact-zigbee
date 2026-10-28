# 5. Risoluzione dei problemi

🌐 [English](../05-troubleshooting.md) · [Français](../fr/05-troubleshooting.md) · **Italiano**

| Sintomo | Causa probabile | Soluzione |
|---|---|---|
| Errore di compilazione *"Select Tools → Zigbee mode…"* | Zigbee mode sbagliata | Tools → Zigbee mode → **Zigbee ZCZR**, Partition → **Zigbee ZCZR 4MB with spiffs** |
| La build fallisce con *ctags: bad CPU type in executable* (Mac Apple Silicon) | Il ctags incluso in Arduino è solo per Intel | Compila con PlatformIO (vedi [Firmware](03-firmware.md)), oppure installa Rosetta: `softwareupdate --install-rosetta --agree-to-license` |
| PlatformIO: *Python version must be 3.10 … 3.14* | Il `platformio` di Homebrew usa un Python più recente | `brew uninstall platformio`, poi `uv tool install --python 3.13 platformio` |
| Errore di compilazione su `requestIASZoneEnroll` / `restoreIASZoneEnroll` | Core ESP32 vecchio | Aggiorna **esp32 by Espressif** alla 3.3.0 o successiva |
| Il LED lampeggia lentamente all'infinito | Non si collega alla rete | Attiva *Permit join* in Z2M; avvicinati a un router; tieni premuto BOOT 3 s per il reset di fabbrica; riflasha con *Erase All Flash = Enabled* |
| Z2M mostra *Unsupported* / *DIY C6ReedContact not supported* | Converter non caricato | Il file deve stare in `external_converters/` con estensione `.mjs`; riavvia Z2M; controlla gli errori nel log |
| Abbinato, ma `contact` non cambia mai | Enrollment IAS non completato | Il log seriale mostra "Enrollment timed out": in Z2M fai *Reconfigure* del dispositivo, oppure rimuovilo, tieni premuto BOOT 3 s e riabbinalo |
| Stato invertito (chiusa risulta aperta) | Reed collegato su **NC** | Sposta il filo su **NO** (vedi la [verifica col multimetro](01-hardware-and-wiring.md#perché-no-e-non-nc)) |
| Lo stato sfarfalla quando il motore gira | Disturbi su un cavo del reed lungo | Aggiungi il pull-up da 10 kΩ + 100 nF; usa cavo intrecciato o schermato; fallo passare lontano dal cavo di rete del motore; aumenta `DEBOUNCE_MS` |
| Risulta *chiusa* con la porta aperta di qualche cm | Distanza del magnete troppo tollerante | Sposta reed/magnete in modo che si allineino solo nell'ultimo centimetro, o riduci la distanza |
| Il sensore si scollega quando il tastierino apre la porta | Alimentato dall'uscita **serratura** dello YAVIS | Passa ai morsetti **costanti** `+12V`/`GND` |
| Lo XIAO si resetta o scalda | Uscita del buck sbagliata | Misura: deve essere 5,0–5,1 V; risigilla il trimmer |
| LQI scarso | Antenna | Controlla che il connettore U.FL sia inserito bene; tieni l'antenna lontana dal buck e dal metallo; verifica che `useExternalAntenna()` venga eseguita |
| Cover sempre *unavailable* in HA | Entity ID diversi | Correggi gli ID in `garage_door.yaml`; i friendly name in Z2M devono essere `garage_door_sensor` e `garage_opener` |

## Righe utili del log seriale

```
Starting Zigbee (router)
Joining network.....
Requesting IAS Zone enrollment     (first pairing)
Restoring IAS Zone enrollment      (after a reboot)
Enrolled
Contact CLOSED / Contact OPEN
```
