# 2. Montage pas à pas

🌐 [English](../02-build-guide.md) · **Français** · [Italiano](../it/02-build-guide.md)

Suivez ces étapes dans l'ordre. Vous testez chaque partie sur l'établi avant qu'elle n'aille dans le garage.

## Étape 1 : flasher et appairer sur l'établi (USB uniquement)

1. Flashez le firmware par USB-C. Voir [Firmware](03-firmware.md).
2. Installez le convertisseur Z2M et appairez la carte. Voir [Zigbee2MQTT et HA](04-zigbee2mqtt-and-home-assistant.md).
3. Branchez l'interrupteur reed sur `D2` / `GND` avec des fils courts. Approchez et éloignez l'aimant et vérifiez que Z2M affiche `contact: true` (aimant proche) et `contact: false` (aimant éloigné).

Vous avez maintenant un capteur fonctionnel avant d'avoir soudé quoi que ce soit de définitif.

## Étape 2 : régler l'abaisseur sur 5,0 V

1. Branchez `IN+`/`IN−` du LM2596 sur l'alimentation 12 V. **Rien sur la sortie pour l'instant.**
2. Placez un multimètre sur `OUT+`/`OUT−`.
3. Tournez le potentiomètre bleu (il faut généralement beaucoup de tours, le sens antihoraire baisse la tension) jusqu'à lire **5,0–5,1 V**.
4. Coupez l'alimentation. Mettez une goutte de vernis à ongles ou de colle chaude sur le potentiomètre pour qu'il ne dérive pas.

> Ne branchez jamais le XIAO avant d'avoir mesuré la sortie de l'abaisseur. Ces modules sont souvent livrés réglés entre 12 et 35 V en sortie.

## Étape 3 : préparer le boîtier

1. Prévoyez l'agencement dans le boîtier IP67 : l'abaisseur d'un côté, le XIAO de l'autre, l'antenne le long du couvercle ou d'une paroi latérale, loin de l'inductance de l'abaisseur.
2. Percez les trous pour **2 presse-étoupes** en **bas** du boîtier : un pour l'arrivée 12 V, un pour le câble du reed. Avec les presse-étoupes en bas, l'eau ne peut pas entrer.
3. Fixez les cartes sur la plaque de fond perforée avec des entretoises nylon M2/M3, ou avec de l'adhésif mousse double face pour le XIAO.
4. Collez l'antenne U.FL à l'intérieur du couvercle ou d'une paroi latérale. Le plastique ABS laisse passer le Zigbee. Ne mettez pas le boîtier **à l'intérieur** du carter métallique du moteur, ni collé contre un cadre de porte métallique.

## Étape 4 : câbler le boîtier

Suivez le [schéma de câblage](01-hardware-and-wiring.md#schéma-de-câblage). Peu habitué à souder, ou des soudures qui ne tiennent pas ? Lisez d'abord le [guide de soudure](06-soldering.md) : il place le XIAO sur une plaque à pastilles avec des borniers à vis, bien plus fiable que des fils soudés directement sur ses pastilles.

1. Soudez la résistance de 10 kΩ entre `D2` et `3V3`, et le condensateur de 100 nF entre `D2` et `GND`, sur les broches du XIAO (ou sur un petit morceau de plaque à trous).
2. Abaisseur `OUT+` → XIAO `5V`, abaisseur `OUT−` → XIAO `GND`. Soudez-les, ne comptez pas sur des cavaliers Dupont lâches dans un garage.
3. Installez le bornier à vis ou le Wago pour le câble du reed (`D2`, `GND`).
4. Installez le bornier à vis ou le Wago pour l'arrivée 12 V, avec le fusible 1 A optionnel sur le `+`.
5. Test sur l'établi : alimentez le boîtier en 12 V. La LED du XIAO clignote lentement pendant qu'il rejoint le réseau, puis s'éteint. Le reed fonctionne toujours dans Z2M.

> Ne branchez pas le XIAO en USB pendant que le boîtier est alimenté en 12 V. Le 5 V de l'USB et le 5 V de l'abaisseur arrivent sur la même broche. Débranchez d'abord le 12 V si vous devez reflasher.

## Étape 5 : fixer l'interrupteur reed sur la porte

L'objectif est de détecter **uniquement la fermeture complète**, afin qu'une porte arrêtée quelques centimètres trop tôt compte toujours comme « pas fermée ».

1. Fermez complètement la porte.
2. Choisissez un endroit où la porte et le cadre ne sont au plus près **qu'en** toute fin de course :
   - **Porte sectionnelle ou basculante :** sur le rail latéral près du sol, ou sur le cadre au niveau du bord supérieur de la porte.
   - **Porte enroulable :** la lame du bas au niveau du sol, avec l'aimant sur la dernière lame et le reed sur la coulisse.
3. Fixez le reed (avec le câble) sur le **cadre fixe**. Fixez l'**aimant** sur la **porte mobile**, aligné avec lui.
4. Gardez un écart inférieur à environ 15 mm. Testez au multimètre, ou surveillez Z2M : ouvrez la porte de 3–5 cm et l'état doit déjà passer à `contact: false`. Sinon, déplacez les pièces pour qu'elles s'alignent plus précisément, ou réduisez l'écart.
5. Faites passer le câble jusqu'au boîtier dans une gaine ou avec des clips, loin du câble d'alimentation du moteur.

## Étape 6 : câbler l'alimentation et la motorisation

1. **Coupez le disjoncteur.**
2. Prenez le 12 V sur les bornes **permanentes** `+12V`/`GND` de la YAVIS. Tirez une paire vers le boîtier du capteur et une paire vers le ZB-SW01 (ou repiquez-les en cascade au niveau du boîtier).
3. ZB-SW01 `COM`/`NO` → bornes bouton-poussoir du moteur, en parallèle avec le bouton mural (sautez cette étape si c'est déjà fait).
4. Serrez tous les presse-étoupes et fermez le couvercle.
5. Remettez le disjoncteur.

## Étape 7 : vérifications finales

| Test | Résultat attendu |
|---|---|
| Porte complètement fermée | HA `Garage door` = **Closed** (fermée) |
| Ouvrir la porte | HA = **Open** (ouverte) en environ 1 s |
| Arrêter la porte 5 cm avant la fermeture | HA reste sur **Open** |
| `cover.close_cover` depuis HA | La porte se ferme, aucune notification |
| Bloquer la porte (ou l'arrêter) pendant une fermeture lancée depuis HA | Notification après 90 s : « still not fully closed » |
| La laisser ouverte 10 min | Notification « left open » |
| Couper le 12 V pendant 20 min | Notification « sensor offline » |
| Carte Z2M | Le capteur apparaît comme **routeur** |
