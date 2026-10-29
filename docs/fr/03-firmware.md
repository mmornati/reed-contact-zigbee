# 3. Firmware : compiler et flasher

🌐 [English](../03-firmware.md) · **Français** · [Italiano](../it/03-firmware.md)

Sketch : [`firmware/reed_contact/reed_contact.ino`](../../firmware/reed_contact/reed_contact.ino)

> **Deux firmwares au choix, le même matériel.** Ce guide décrit le firmware Arduino (options A et B). Si vous gérez déjà vos appareils avec ESPHome, vous pouvez flasher [`esphome/reed-contact.yaml`](../../esphome/reed-contact.yaml) à la place : la section *Option C : ESPHome* plus bas explique les différences et comment le flasher. Le câblage est identique dans les deux cas.

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

## Option C : ESPHome

Si vous gérez déjà vos appareils avec [ESPHome](https://esphome.io/), vous pouvez flasher [`esphome/reed-contact.yaml`](../../esphome/reed-contact.yaml) à la place du sketch. Il utilise le support Zigbee natif d'ESPHome pour l'ESP32-C6 : c'est toujours un routeur Zigbee, sans WiFi. **Le câblage est exactement le même** : reed sur D2 / GPIO2 vers GND, antenne externe via GPIO3 / GPIO14, bouton BOOT sur GPIO9, LED sur GPIO15.

### Firmware Arduino ou ESPHome ?

| | Firmware Arduino (options A, B) | ESPHome (option C) |
|---|---|---|
| Cluster Zigbee | Contact **IAS Zone**, le standard des capteurs de porte/fenêtre | **Binary Input** : ESPHome ne gère pas encore l'IAS Zone |
| Convertisseur Zigbee2MQTT | [`reed_contact.mjs`](../../zigbee2mqtt/reed_contact.mjs) | [`reed_contact_esphome.mjs`](../../zigbee2mqtt/reed_contact_esphome.mjs) |
| Résultat dans Z2M et Home Assistant | `contact`, `binary_sensor` de classe *door* | le même |
| ZHA et autres coordinateurs | Reconnu comme capteur d'ouverture | Vu comme une entrée binaire générique |
| Chaîne de modèle | `C6ReedContact` | `C6ReedContactESPHome` |
| Flasheur web et binaires des releases | Oui | Non, vous le compilez avec ESPHome |
| Mises à jour | USB | USB. Par Zigbee uniquement avec la version OTA expérimentale (voir plus bas) |
| Version d'ESPHome | | branche `dev` (2026.11 ou plus récente) |
| Personnalisation | Modifier le sketch C++ | Modifier le YAML : un second reed, un relais ou une sonde de température tiennent en quelques lignes |

Choisissez le firmware Arduino si vous voulez le capteur d'ouverture standard et le flash en un clic. Choisissez ESPHome si vous préférez le YAML et avez déjà une méthode de travail ESPHome.

### Ce que fait le YAML

Tout ce que fait le sketch, sauf l'enrôlement IAS Zone, inutile pour une Binary Input :

- **Routeur** Zigbee, alimenté sur secteur, fabricant `DIY`, modèle `C6ReedContactESPHome`.
- Bascule sur l'antenne externe U.FL au démarrage.
- Lit le reed sur **GPIO2** avec la résistance de pull-up interne et un anti-rebond de **200 ms**. `presentValue` vaut `true` quand le reed est ouvert (aimant éloigné ou fil coupé) et `false` quand il est fermé : une panne se lit toujours comme *ouvert*.
- Renvoie l'état **toutes les 5 minutes** (heartbeat).
- **LED :** clignotement lent pendant la recherche d'un réseau, éteinte une fois le réseau rejoint.
- **Réinitialisation d'usine :** maintenez **BOOT** 3 s, **puis relâchez**. La carte quitte le réseau et redémarre prête à être appairée. Le firmware Arduino se réinitialise pendant que vous maintenez le bouton ; ESPHome attend le relâchement.

### Compiler et flasher

1. Installez ESPHome depuis la branche `dev`. Le Zigbee natif et l'option `manufacturer` ne sont pas encore dans une version stable :

   ```bash
   uv tool install https://github.com/esphome/esphome/archive/dev.zip
   ```

   Ou `pip install https://github.com/esphome/esphome/archive/dev.zip` dans un virtualenv, ou l'image Docker `ghcr.io/esphome/esphome:dev`.
2. Branchez le XIAO en USB-C (**sans** 12 V sur le buck). S'il exécutait le firmware Arduino, effacez-le d'abord pour ne pas garder d'anciennes données de réseau Zigbee :

   ```bash
   esptool --chip esp32c6 erase-flash
   ```

3. Compilez et téléversez (le premier lancement télécharge la chaîne de compilation ESP-IDF) :

   ```bash
   esphome run esphome/reed-contact.yaml
   ```

   Choisissez le port série USB quand il est demandé. Si le port n'est pas trouvé : maintenez **BOOT**, appuyez sur **RESET**, relâchez **BOOT**, puis réessayez.
4. Suivez les logs avec `esphome logs esphome/reed-contact.yaml`.
5. Installez [`zigbee2mqtt/reed_contact_esphome.mjs`](../../zigbee2mqtt/reed_contact_esphome.mjs) comme convertisseur Z2M, puis appairez comme décrit dans [le guide suivant](04-zigbee2mqtt-and-home-assistant.md).

### Paramètres que vous pourriez modifier

| Define Arduino | Dans le YAML |
|---|---|
| `REED_PIN` | `binary_sensor` → `contact` → `pin` → `number` |
| `DEBOUNCE_MS` | `filters` → `delayed_on_off` |
| `HEARTBEAT_MS` | l'entrée `5min` sous `interval` |
| `FACTORY_RESET_HOLD_MS` | `on_click` → `min_length` |
| `useExternalAntenna()` | le bloc `on_boot` (supprimez-le pour utiliser l'antenne céramique) |

### Mettre à jour une version ESPHome

L'OTA habituel d'ESPHome passe par le WiFi, que cette version n'utilise pas. Débranchez le 12 V, branchez l'USB et relancez `esphome run` **sans effacer** : la carte conserve ses clés réseau et se reconnecte sans nouvel appairage.

Les mises à jour par Zigbee sont possibles avec un composant expérimental `ota: platform: zigbee`, en revue dans [mmornati/esphome#1](https://github.com/mmornati/esphome/pull/1) et pas encore intégré à ESPHome. Avec lui, Zigbee2MQTT envoie le nouveau firmware à travers le maillage. C'est lent (environ 30 minutes pour 360 Ko), prévoyez-le.

### Passer d'une version à l'autre

La chaîne de modèle change, donc Z2M voit un nouvel appareil. Supprimez-le dans Z2M, flashez l'autre version avec un effacement complet, appairez-le à nouveau et donnez-lui le même nom (`garage_door_sensor`). Home Assistant garde les mêmes identifiants d'entité.
