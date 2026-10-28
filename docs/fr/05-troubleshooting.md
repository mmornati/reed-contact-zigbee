# 5. Dépannage

🌐 [English](../05-troubleshooting.md) · **Français** · [Italiano](../it/05-troubleshooting.md)

| Symptôme | Cause probable | Solution |
|---|---|---|
| Erreur de compilation *"Select Tools → Zigbee mode…"* | Mauvais mode Zigbee | Tools → Zigbee mode → **Zigbee ZCZR**, Partition → **Zigbee ZCZR 4MB with spiffs** |
| La compilation échoue avec *ctags: bad CPU type in executable* (Mac Apple Silicon) | Le ctags fourni avec Arduino n'existe qu'en version Intel | Compilez plutôt avec PlatformIO (voir [Firmware](03-firmware.md)), ou installez Rosetta : `softwareupdate --install-rosetta --agree-to-license` |
| PlatformIO : *Python version must be 3.10 … 3.14* | Le `platformio` de Homebrew utilise un Python plus récent | `brew uninstall platformio`, puis `uv tool install --python 3.13 platformio` |
| Erreur de compilation sur `requestIASZoneEnroll` / `restoreIASZoneEnroll` | Core ESP32 trop ancien | Mettez à jour **esp32 by Espressif** en 3.3.0 ou plus récent |
| La LED clignote lentement indéfiniment | N'arrive pas à rejoindre le réseau | Activez *Permit join* dans Z2M ; rapprochez-vous d'un routeur ; maintenez BOOT 3 s pour une réinitialisation d'usine ; reflashez avec *Erase All Flash = Enabled* |
| Z2M affiche *Unsupported* / *DIY C6ReedContact not supported* | Convertisseur non chargé | Le fichier doit être dans `external_converters/` avec l'extension `.mjs` ; redémarrez Z2M ; cherchez des erreurs dans le log |
| Appairé, mais `contact` ne change jamais | L'enrôlement IAS n'a pas abouti | Le log série affiche "Enrollment timed out" : dans Z2M, faites *Reconfigure* sur l'appareil, ou supprimez-le, maintenez BOOT 3 s et appairez de nouveau |
| État inversé (fermé affiché ouvert) | Reed câblé sur **NC** | Déplacez le fil sur **NO** (voir la [vérification au multimètre](01-hardware-and-wiring.md#pourquoi-no-et-pas-nc-)) |
| L'état clignote quand le moteur tourne | Bruit sur un long câble de reed | Ajoutez le pull-up 10 kΩ + 100 nF ; utilisez un câble torsadé ou blindé ; éloignez-le du câble secteur du moteur ; augmentez `DEBOUNCE_MS` |
| Affiche *fermé* alors que la porte est ouverte de quelques cm | Écart de l'aimant trop tolérant | Déplacez le reed/l'aimant pour qu'ils ne s'alignent qu'au dernier centimètre, ou réduisez l'écart |
| Le capteur décroche quand le clavier ouvre la porte | Alimenté par la sortie **gâche** de la YAVIS | Passez sur les bornes **permanentes** `+12V`/`GND` |
| Le XIAO redémarre ou chauffe | Mauvaise tension en sortie de l'abaisseur | Mesurez : elle doit être de 5,0–5,1 V ; rebloquez le potentiomètre |
| LQI faible | Antenne | Vérifiez que le connecteur U.FL est bien enfoncé ; éloignez l'antenne de l'abaisseur et du métal ; vérifiez que `useExternalAntenna()` est bien exécuté |
| Cover toujours *unavailable* dans HA | Les identifiants d'entité diffèrent | Corrigez les identifiants dans `garage_door.yaml` ; les friendly names Z2M doivent être `garage_door_sensor` et `garage_opener` |

## Lignes utiles du log série

```
Starting Zigbee (router)
Joining network.....
Requesting IAS Zone enrollment     (first pairing)
Restoring IAS Zone enrollment      (after a reboot)
Enrolled
Contact CLOSED / Contact OPEN
```
