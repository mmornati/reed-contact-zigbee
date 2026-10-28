# 3. Firmware : compiler et flasher

🌐 [English](../03-firmware.md) · **Français** · [Italiano](../it/03-firmware.md)

Sketch : [`firmware/reed_contact/reed_contact.ino`](../../firmware/reed_contact/reed_contact.ino)

## Ce qu'il fait

- Démarre en tant que **routeur Zigbee** : il reste toujours allumé et relaie le trafic des autres appareils.
- Bascule la radio sur l'**antenne externe U.FL** (GPIO3 = LOW, GPIO14 = HIGH).
- Expose un endpoint **contact IAS Zone** (fabricant `DIY`, modèle `C6ReedContact`).
- Lit le reed sur **D2 / GPIO2** : `LOW` = complètement fermé, `HIGH` = pas fermé. Anti-rebond de 200 ms.
- Envoie l'état à chaque changement, et de nouveau **toutes les 5 minutes** comme heartbeat.
- Gère l'enrôlement (enrollment) IAS Zone. Il mémorise l'enrôlement entre les redémarrages et réessaie chaque minute si le coordinateur n'a pas répondu.
- **LED :** clignotement lent = recherche d'un réseau, éteinte = réseau rejoint. Tant que vous maintenez BOOT, elle clignote rapidement.
- **Réinitialisation d'usine :** maintenez **BOOT** pendant 3 s. La carte quitte le réseau, oublie l'enrôlement et redémarre prête à être appairée.

## Option A : PlatformIO (recommandé, natif sur Apple Silicon)

[`firmware/platformio.ini`](../../firmware/platformio.ini) contient tous les paramètres de compilation (carte, mode routeur Zigbee, table de partitions), il n'y a donc rien à choisir dans des menus. Il utilise la plateforme communautaire [pioarduino](https://github.com/pioarduino/platform-espressif32), qui embarque le core arduino-esp32 3.x. La plateforme PlatformIO officielle s'arrête au core 2.x, qui n'a pas de Zigbee.

1. Installez PlatformIO Core. Il lui faut Python 3.10–3.14 ; le `platformio` de Homebrew tourne sur un Python plus récent que pioarduino refuse, utilisez donc uv :

   ```bash
   uv tool install --python 3.13 platformio
   ```

   Ou installez l'extension *PlatformIO IDE* dans VS Code, qui apporte son propre Python.
2. Compilez (la première exécution télécharge la plateforme ESP32 et la chaîne de compilation, quelques centaines de Mo) :

   ```bash
   cd firmware
   pio run
   ```

3. Branchez le XIAO en USB-C (**sans** 12 V sur l'abaisseur). Pour le tout premier flashage, effacez-le, puis téléversez :

   ```bash
   pio run -t erase
   pio run -t upload
   ```

   Si le port n'est pas trouvé : maintenez **BOOT**, appuyez brièvement sur **RESET**, relâchez **BOOT**, puis relancez le téléversement.
4. Suivez les logs :

   ```bash
   pio device monitor
   ```

## Option B : Arduino IDE

> Sur un Mac Apple Silicon, l'Arduino IDE a besoin de Rosetta : son outil de prétraitement ctags n'existe qu'en version Intel et échoue avec *"bad CPU type in executable"*. Installez-le avec `softwareupdate --install-rosetta --agree-to-license`, ou utilisez PlatformIO ci-dessus.

1. Installez **Arduino IDE 2.x**.
2. *File → Preferences → Additional boards manager URLs* (URL de gestionnaire de cartes supplémentaires) :
   `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json`
3. *Boards Manager* → installez **esp32 by Espressif Systems**, version **3.3.0 ou plus récente**.
4. Ouvrez `firmware/reed_contact/reed_contact.ino`.
5. Menu *Tools* (Outils) :

| Paramètre | Valeur |
|---|---|
| Board | **XIAO_ESP32C6** |
| Zigbee mode | **Zigbee ZCZR (coordinator/router)** |
| Partition Scheme | **Zigbee ZCZR 4MB with spiffs** |
| Erase All Flash Before Sketch Upload | **Enabled** pour le premier flashage, puis Disabled |
| USB CDC On Boot | Enabled (pour voir les logs) |

Si vous choisissez le mauvais mode Zigbee, le sketch refuse de compiler avec un message explicite.

Ensuite, flashez :

1. Branchez le XIAO en USB-C (**sans** 12 V sur l'abaisseur).
2. Cliquez sur **Upload**. Si le port n'apparaît pas : maintenez **BOOT**, appuyez brièvement sur **RESET**, relâchez **BOOT**, puis relancez le téléversement.
3. Ouvrez le Serial Monitor à 115200 bauds.

## Premier démarrage

Quel que soit l'outil utilisé, le log série doit afficher :

```
Starting Zigbee (router)
Joining network.......
```

Appairez-le maintenant dans Zigbee2MQTT. Voir [le guide suivant](04-zigbee2mqtt-and-home-assistant.md).

## Paramètres que vous pourriez modifier

En haut du sketch :

| Define | Défaut | Signification |
|---|---|---|
| `REED_PIN` | `2` (D2) | Broche d'entrée du reed |
| `DEBOUNCE_MS` | `200` | Durée pendant laquelle le reed doit rester stable |
| `HEARTBEAT_MS` | 5 min | Renvoi périodique de l'état |
| `FACTORY_RESET_HOLD_MS` | `3000` | Durée d'appui sur BOOT pour la réinitialisation |

Pour utiliser plutôt l'antenne céramique intégrée, supprimez l'appel à `useExternalAntenna()`.

## Mises à jour ultérieures

Débranchez le 12 V, branchez l'USB et téléversez **sans effacer** : un simple `pio run -t upload`, ou dans l'Arduino IDE avec *Erase All Flash = Disabled*. La carte conserve ses clés réseau et son enrôlement, et se reconnecte d'elle-même sans nouvel appairage.
