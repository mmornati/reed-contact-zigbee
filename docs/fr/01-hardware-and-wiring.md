# 1. Matériel et câblage

🌐 [English](../01-hardware-and-wiring.md) · **Français** · [Italiano](../it/01-hardware-and-wiring.md)

> ⚠️ L'alimentation YAVIS est branchée sur le secteur (90–260 V AC). Coupez le disjoncteur avant de toucher à ses bornes d'entrée. Tout le reste de ce projet est en très basse tension (12 V / 5 V / 3,3 V).

## Bilan de consommation

| Charge | Typique | Pic |
|---|---|---|
| XIAO ESP32-C6, routeur Zigbee (radio toujours active) | ~30 mA @ 5 V | ~120 mA @ 5 V |
| Relais ZB-SW01 | ~30 mA @ 12 V | ~80 mA @ 12 V (bobine active) |
| Kit de contrôle d'accès (clavier + ventouse 180 kg) | ~500 mA @ 12 V | ~600 mA |

Le total reste bien en dessous de 1 A. L'alimentation de 5 A a une marge confortable.

## Réponses aux questions d'alimentation

- **Le ZB-SW01 directement en 12 V ?** Oui. Il accepte 5–32 V DC sur ses bornes d'alimentation. Vérifiez l'étiquette de votre exemplaire (certains lots indiquent « DC 7–32 V » ou acceptent aussi l'AC) et respectez le marquage +/−.
- **Le XIAO directement en 12 V ?** **Non.** La broche 5 V du XIAO doit rester à environ 5 V, l'abaisseur LM2596 est donc indispensable. Envoyez les 5,0 V de l'abaisseur sur la broche **5V** du XIAO, pas sur 3V3. Le régulateur intégré du XIAO fournit ensuite un 3,3 V propre.
- **Quelles bornes de la YAVIS ?** Les alimentations de contrôle d'accès ont une sortie DC **permanente** (`+12V`/`DC+` et `GND`/`DC−`) et une sortie **commutée pour la gâche** (`NO`/`COM`/`NC`, `PUSH`, temporisation). N'utilisez que les bornes DC permanentes. La sortie gâche se coupe à chaque déverrouillage et ferait redémarrer le capteur.

### Ai-je besoin de la batterie ?

Non. Le capteur fonctionne sur secteur en tant que routeur Zigbee, ce qui est la meilleure configuration : pas de batterie à recharger, et il renforce le maillage. Une LiPo sur les pastilles batterie du XIAO ne servirait que de secours pendant une coupure de courant, mais le moteur de porte et la motorisation s'arrêtent aussi pendant une coupure, et les cellules LiPo n'aiment pas un garage froid en hiver et chaud en été. Gardez-la pour un autre projet.

## Schéma de câblage

Le même câblage convient aux deux firmwares : le sketch Arduino et le [YAML ESPHome](03-firmware.md) (option C) utilisent les mêmes broches (reed sur D2 / GPIO2, commutateur d'antenne sur GPIO3 / GPIO14, BOOT sur GPIO9, LED sur GPIO15).

```
          230 V AC                                   GARAGE DOOR MOTOR
             │                                      ┌────────────────────┐
     ┌───────┴────────┐                             │  push-button input │
     │ YAVIS 12V PSU  │                             │  (START/PB  COM)   │
     │                │                             └─────┬────────┬─────┘
     │  +12V   GND    │                                   │        │   ← in parallel with the
     └───┬──────┬─────┘                                   │        │     existing wall button
         │      │                                     ┌───┴────────┴───┐
         │      ├────────────────────────────────────►│ −   ZB-SW01    │
         ├──[F 1A]───────────────────────────────────►│ +   NO   COM   │
         │      │                                     └────────────────┘
         │      │       ┌──────────────────┐
         ├─────────────►│ IN+   LM2596     │
         │      └──────►│ IN−   set 5.0 V  │
         │              │ OUT+       OUT−  │
         │              └──┬──────────┬────┘
         │                 │          │
         │          ┌──────┴──────────┴───────────────┐
         │          │   5V           GND               │
         │          │                                  │
         │          │      Seeed XIAO ESP32-C6         │
         │          │                                  │
         │          │   3V3 ──[10 kΩ]──┐               │
         │          │                  │               │
         │          │   D2 (GPIO2) ────┼──[100 nF]── GND
         │          │                  │               │
         │          │   GND            │      U.FL ◄── external antenna
         │          └────┬─────────────┼───────────────┘
         │               │             │
         │               │   twisted pair to the door
         │               │             │
         │            ┌──┴─────────────┴──┐
         │            │ COM      NO       │  Reed switch (frame)
         │            └───────────────────┘
         │                  ▲ magnet (door): within ~15 mm when FULLY closed
```

### Tableau des connexions

| De | Vers | Fil |
|---|---|---|
| YAVIS `+12V` (permanent) | Fusible 1 A → LM2596 `IN+` | rouge, 0,5 mm² |
| YAVIS `GND` | LM2596 `IN−` | noir, 0,5 mm² |
| YAVIS `+12V` (permanent) | ZB-SW01 `+` / `DC+` | rouge |
| YAVIS `GND` | ZB-SW01 `−` / `DC−` | noir |
| LM2596 `OUT+` (**5,0 V**) | XIAO `5V` | rouge |
| LM2596 `OUT−` | XIAO `GND` | noir |
| Reed `COM` | XIAO `GND` | conducteur 1 du câble |
| Reed `NO` | XIAO `D2` (GPIO2) | conducteur 2 du câble |
| Résistance 10 kΩ | entre XIAO `D2` et `3V3` | côté XIAO |
| Condensateur 100 nF | entre XIAO `D2` et `GND` | côté XIAO |
| ZB-SW01 `COM` + `NO` | bornes bouton-poussoir du moteur | selon la notice du moteur |
| Antenne U.FL | connecteur d'antenne du XIAO | |

### Pourquoi NO et pas NC ?

Avec le contact **NO** (normalement ouvert) du reed, l'interrupteur ne se ferme que lorsque l'aimant est en face, c'est-à-dire quand la porte est complètement fermée. Le XIAO voit alors `D2 = LOW`, ce qui signifie fermé. Toute défaillance (porte entrouverte, câble coupé, connecteur desserré, capteur mort) donne `D2 = HIGH`, soit **pas fermé** : vous recevez une alerte au lieu d'un faux « tout va bien ».

Vérifiez-le au multimètre avant le montage. Ces reeds bon marché n'identifient pas toujours les fils de la même façon :

1. Mettez le multimètre en mode continuité.
2. Trouvez la paire qui **bipe uniquement quand l'aimant est proche**. Ce sont COM + NO.
3. La paire qui bipe quand l'aimant est **éloigné** est COM + NC. Laissez NC non connecté et isolé.

### Pourquoi 10 kΩ + 100 nF ?

Le firmware active déjà le pull-up interne de l'ESP32 (environ 45 kΩ). Sur quelques mètres de câble qui longent un moteur, c'est faible et ça capte du bruit. Un pull-up externe de 10 kΩ plus 100 nF vers la masse rendent le signal solide. Le firmware applique en plus un anti-rebond de 200 ms.

## Le ZB-SW01 sur le moteur

- Câblez le **COM / NO** du ZB-SW01 sur l'entrée bouton-poussoir du moteur (souvent marquée `START`, `PP`, `P/P`, `PB`, `OPEN` + `COM`/`GND`). Mettez-le **en parallèle** avec le bouton mural existant, pour que les deux continuent de fonctionner.
- Utilisez le **mode impulsionnel (inching)** si l'appareil le propose. Le script HA le coupe de toute façon après 800 ms, donc ça marche dans les deux cas.
- Ne branchez jamais les contacts du ZB-SW01 sur les bornes de puissance du moteur. C'est un contact sec qui ne fait que simuler un appui sur un bouton.
