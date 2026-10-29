# 4. Zigbee2MQTT et Home Assistant

🌐 [English](../04-zigbee2mqtt-and-home-assistant.md) · **Français** · [Italiano](../it/04-zigbee2mqtt-and-home-assistant.md)

## 4.1 Installer le convertisseur externe (Zigbee2MQTT 2.x)

Z2M ne connaît pas un appareil DIY : un petit convertisseur lui dit « ceci est un capteur d'ouverture ».

1. Copiez [`zigbee2mqtt/reed_contact.mjs`](../../zigbee2mqtt/reed_contact.mjs) dans le dossier de données de Z2M, sous `external_converters/` :
   - Add-on HA : `/config/zigbee2mqtt/external_converters/reed_contact.mjs` (utilisez l'add-on File editor ou Samba)
   - Docker : `<your data volume>/external_converters/reed_contact.mjs`
2. Redémarrez Zigbee2MQTT. Dans Z2M 2.x, tout ce qui se trouve dans `external_converters/` est chargé automatiquement, sans modifier `configuration.yaml`.
3. Cherchez dans le log de Z2M une ligne indiquant que le convertisseur externe `reed_contact.mjs` a été chargé.

> **Version ESPHome ?** Utilisez plutôt [`zigbee2mqtt/reed_contact_esphome.mjs`](../../zigbee2mqtt/reed_contact_esphome.mjs), de la même manière. ESPHome expose le reed comme une Binary Input et non comme une IAS Zone, avec le modèle `C6ReedContactESPHome` : les deux convertisseurs peuvent cohabiter dans `external_converters/`.

## 4.2 Appairer le capteur

1. Dans l'interface de Z2M, cliquez sur **Permit join (All)** (autoriser l'appairage). Choisir le coordinateur ou un routeur proche du garage aide.
2. Alimentez le XIAO (USB sur l'établi, ou 12 V une fois installé). La LED clignote lentement, puis s'éteint une fois le réseau rejoint.
3. Z2M l'interroge (interview) et doit afficher **DIY C6ReedContact, "Mains-powered reed contact sensor and Zigbee router (XIAO ESP32-C6)"**, type **Router**. La version ESPHome apparaît comme **DIY C6ReedContactESPHome**.
4. Renommez-le **`garage_door_sensor`** et cochez *Update Home Assistant entity ID*.
5. Vérifiez l'onglet *Exposes* : `contact` doit être à **true** avec l'aimant en face et à **false** sans lui.

S'il apparaît comme « Unsupported », le convertisseur n'est pas chargé ou la chaîne de modèle ne correspond pas. Voir [Dépannage](05-troubleshooting.md).

### Réglages Z2M recommandés

- *Settings → Availability* activé. Les routeurs sont interrogés activement, donc HA voit `unavailable` si le capteur meurt.
- Le ZB-SW01 doit s'appeler **`garage_opener`**, ce qui donne `switch.garage_opener`.

## 4.3 Entités dans Home Assistant

Grâce à la découverte MQTT, vous obtenez :

| Entité | Signification |
|---|---|
| `binary_sensor.garage_door_sensor_contact` | classe d'appareil *door* : **on = pas complètement fermée**, **off = fermée** |
| `sensor.garage_door_sensor_linkquality` | LQI Zigbee (désactivée par défaut) |
| `switch.garage_opener` | Relais ZB-SW01 |

> Vérifiez les vrais identifiants d'entité dans *Settings → Devices → garage_door_sensor* (Paramètres → Appareils). S'ils diffèrent, faites un rechercher-remplacer dans le package ci-dessous.

## 4.4 Installer le package HA

[`homeassistant/garage_door.yaml`](../../homeassistant/garage_door.yaml) contient :

| Élément | Ce qu'il fait |
|---|---|
| `script.garage_door_pulse` | Envoie une impulsion de 800 ms au ZB-SW01, comme un appui sur le bouton mural |
| `script.garage_door_open` | N'envoie l'impulsion que si la porte est fermée |
| `script.garage_door_close` | N'envoie l'impulsion que si la porte est ouverte, puis **vérifie qu'elle est complètement fermée en moins de 90 s**, et vous prévient sinon |
| `cover.garage_door` | Template cover (classe d'appareil *garage*) avec boutons d'ouverture/fermeture et l'état du capteur |
| `input_number.garage_door_open_alert_minutes` | Délai avant l'alerte « restée ouverte » (10 min par défaut) |
| Automatisation *left open* | Prévient quand la porte n'est pas complètement fermée depuis N minutes |
| Automatisation *night check* | Prévient à 22 h si la porte n'est pas fermée |
| Automatisation *sensor offline* | Prévient si le capteur est indisponible depuis 15 min |

Étapes :

1. Dans `configuration.yaml`, si ce n'est pas déjà présent :

   ```yaml
   homeassistant:
     packages: !include_dir_named packages
   ```

2. Copiez le fichier dans `/config/packages/garage_door.yaml`.
3. Remplacez `notify.notify` par votre service de notification, par ex. `notify.mobile_app_<your_phone>` ou votre notifier ntfy.
4. *Developer tools → YAML → Check configuration* (Outils de développement), puis **Restart**. Les template covers et les nouveaux packages exigent un redémarrage, pas un simple rechargement.
5. Ajoutez `cover.garage_door` à un tableau de bord. Une carte *Tile* avec la fonction d'ouverture/fermeture du cover fonctionne bien.

### Pourquoi le cover n'a pas de « stop »

Le ZB-SW01 n'a qu'un seul bouton : impulsion = ouvrir / stop / fermer, selon le moteur. Avec un seul capteur « fermé », HA ne peut pas savoir si la porte est en mouvement, donc un bouton stop aurait un comportement imprévisible. Les scripts d'ouverture/fermeture n'envoient l'impulsion que quand c'est sûr : ouvrir seulement si fermée, fermer seulement si pas fermée.

### La vérification « pas complètement fermée »

C'est l'objectif principal du projet. Il est couvert de trois façons :

1. **Complètement fermée** est le seul état qui donne `off`. Tout écart de quelques centimètres donne `on` (pas fermée).
2. Après une fermeture demandée depuis HA, `script.garage_door_close` attend 90 s et vous prévient si la porte n'est toujours pas fermée. Cela détecte un obstacle, l'inversion de sécurité du moteur, ou une porte arrêtée à mi-course.
3. Quoi qu'ait ouvert la porte (clavier, télécommande, bouton mural, HA), l'automatisation *left open* vous prévient après N minutes, et la vérification de 22 h sert de dernier rappel.

## 4.5 Idées pour plus tard

- Une **notification actionnable** sur l'alerte « restée ouverte » avec un bouton *Close now* (fermer maintenant) qui appelle `script.garage_door_close`.
- Fermeture automatique la nuit, uniquement si un détecteur de mouvement indique que personne n'est dans le garage.
- Un second reed en position complètement ouverte (il y en a un de rechange dans le kit) pour obtenir les états ouverte / fermée / en ouverture / en fermeture. Il faut pour cela un second endpoint dans le firmware.
