# 6. Souder le XIAO

🌐 [English](../06-soldering.md) · **Français** · [Italiano](../it/06-soldering.md)

Le XIAO ESP32-C6 est petit : ses pastilles sont espacées de 2,54 mm et `5V`, `GND` et `3V3` sont côte à côte. La plupart des problèmes du type « ça marche, puis plus » viennent d'une mauvaise soudure sur ces pastilles ou d'un fil qui casse à la sortie de la soudure. Ce guide propose un montage qui évite les deux.

## En bref

1. **Ne soudez pas les fils de terrain sur le XIAO.** Soudez des barrettes de broches sur le XIAO, fixez-le sur une petite plaque à pastilles et amenez le câble du reed et le 5 V sur des **borniers à vis** de cette plaque.
2. **Du flux, une panne propre et 2–3 secondes par soudure.** Si l'étain ne coule pas dans ce délai, arrêtez, ajoutez du flux et recommencez. Ne continuez pas à chauffer.
3. **Mesurez avant d'alimenter.** Vérifiez au multimètre l'absence de court-circuit entre `5V`, `3V3` et `GND` après chaque soudure.

## Outils et matériel

| Élément | Recommandé | Pourquoi |
|---|---|---|
| Fer à souder | Régulé en température, 40–80 W, petite panne biseautée (1,5–2 mm) | Une panne conique « aiguille » garde trop peu de chaleur pour les broches |
| Température | **330–350 °C** avec étain au plomb, **350–370 °C** sans plomb | Plus chaud, le flux brûle avant que l'étain coule et les pastilles peuvent se décoller |
| Étain | 0,5–0,8 mm, avec âme de flux. Le 63/37 au plomb est le plus facile | Un fil épais crée des ponts sur un pas de 2,54 mm |
| Flux | Stylo ou gel « no-clean » | Le remède principal aux soudures ternes et granuleuses |
| Nettoyage de panne | Laine de laiton | Une éponge mouillée refroidit la panne et l'use plus vite |
| Support | Troisième main, ou une breadboard pour tenir les barrettes droites | Les deux mains libres, broches à angle droit |
| Plaque à pastilles | Petit morceau, pas de 2,54 mm | Porte le XIAO, la résistance, le condensateur et les borniers |
| Borniers à vis | 2 points, pas de 5,08 mm ou 3,5 mm, ×2 | Câble du reed et entrée 5 V |
| Multimètre | Avec testeur de continuité (bip) | Toutes les vérifications ci-dessous |
| Alcool isopropylique + vieille brosse à dents | 90 % ou plus | Nettoie les résidus de flux entre les pastilles |

## Les broches du XIAO utilisées

Vue de dessus, USB-C en haut :

```
            ┌──[ USB-C ]──┐
   D0  ●    │             │    ●  5V    ← OUT+ du buck
   D1  ●    │   XIAO      │    ●  GND   ← OUT− du buck, COM du reed, 100 nF
   D2  ●    │  ESP32-C6   │    ●  3V3   ← 10 kΩ
   D3  ●    │             │    ●  D10
   D4  ●    │             │    ●  D9
   D5  ●    │             │    ●  D8
   D6  ●    │     U.FL ◯  │    ●  D7
            └─────────────┘
   D2 ← NO du reed, 10 kΩ, 100 nF
```

Seules quatre broches servent : `D2`, `5V`, `GND`, `3V3`. `5V`/`GND`/`3V3` sont trois pastilles voisines, un pont entre elles est donc l'erreur classique. Ne touchez pas aux pastilles de batterie au dos.

## Montage conseillé : le XIAO sur une plaque à pastilles

```
   plaque à pastilles (vue de dessus)
   ┌───────────────────────────────────────────────┐
   │   D2 ●─────┬───────────[10 kΩ]────────● 3V3   │
   │            │                                  │
   │          [100 nF]                             │
   │            │                                  │
   │  GND ●─────┴──────────────────────────● GND   │
   │                                               │
   │  [bornier : D2 | GND]  ← câble du reed        │
   │  [bornier : 5V | GND]  ← sortie du buck       │
   └───────────────────────────────────────────────┘
      XIAO enfiché sur barrettes au milieu
```

`D2` et `3V3` sont de part et d'autre du XIAO : c'est pourquoi la résistance va sur la plaque et non en travers de la carte.

## Pas à pas

Débranchez l'USB et le 12 V avant de commencer. Ne soudez jamais une carte alimentée.

### 1. Barrettes sur le XIAO

1. Enfoncez deux barrettes mâles de 7 broches dans une breadboard, côté long vers le bas, espacées de la largeur du XIAO. La breadboard les garde droites.
2. Posez le XIAO dessus, composants vers le haut.
3. Mettez du flux sur les pastilles. Soudez **une broche d'angle** de chaque côté, puis vérifiez que la carte est bien à plat. Sinon, réchauffez cette broche en appuyant sur la carte.
4. Soudez le reste. Posez la panne **sur la pastille et la broche ensemble**, attendez 1 seconde, amenez l'étain dans la soudure (pas sur la panne), retirez l'étain puis le fer. 2–3 s au total.
5. Une bonne soudure est un petit cône brillant qui enveloppe la broche et couvre la pastille. Une boule posée dessus, ou une surface grise et terne, est une soudure froide : ajoutez du flux et réchauffez.

> Pour éviter cette étape, Seeed vend aussi le XIAO avec les barrettes déjà soudées.

### 2. Composants sur la plaque

1. Soudez des barrettes femelles sur la plaque pour le XIAO (il reste amovible pour le reflasher), ou soudez directement ses barrettes.
2. Placez la **10 kΩ** entre la rangée `D2` et la rangée `3V3`, et le **100 nF** entre la rangée `D2` et une rangée `GND`. Pliez les pattes à plat au dos et servez-vous-en comme pistes.
3. Placez les deux borniers et reliez-les à `D2`/`GND` et `5V`/`GND` avec les pattes des composants ou de courts bouts de fil rigide.
4. Gardez au moins un trou entre les pistes `5V` et `GND`. Coupez les pattes en trop pour qu'elles ne puissent pas se toucher plus tard.

### 3. Fils dans les borniers

1. Utilisez du fil **souple** (multibrin) pour le câble du reed et l'alimentation 5 V. Le fil rigide casse après quelques mois de vibrations.
2. Dénudez 6–7 mm. Utilisez des embouts, ou torsadez les brins et serrez-les **sans** les étamer : un fil étamé dans un bornier à vis se desserre avec le temps.
3. Si vous devez souder un fil sur les sorties du reed, enfilez d'abord une **gaine thermorétractable**, étamez les deux bouts, joignez-les, puis rétractez la gaine sur la soudure. Ajoutez un point de colle chaude ou un collier pour que le câble tire sur le collier et non sur la soudure.

### 4. Nettoyage

Frottez les soudures avec de l'alcool isopropylique et une brosse à dents. Des résidus de flux entre `D2` et `GND` peuvent laisser passer un peu de courant dans un garage humide et rendre l'entrée instable.

## Vérifier avant d'alimenter

Au multimètre, hors tension, XIAO en place :

| Mesure entre | Attendu | Sinon |
|---|---|---|
| `5V` et `GND` | Pas de bip (pas de court-circuit) | Cherchez un pont sur les pastilles de droite ou sur le bornier |
| `3V3` et `GND` | Pas de bip | Idem |
| `D2` et `3V3` | Environ **10 kΩ** (un peu moins est normal avec le XIAO en place) | Soudure de la résistance ouverte, ou mauvaise rangée |
| `D2` et `GND`, aimant devant le reed | Bip (environ 0 Ω) | Reed câblé sur NC au lieu de NO, ou soudure cassée sur le câble |
| `D2` et `GND`, aimant éloigné | Pas de bip | Mauvaise paire de fils du reed, ou pont `D2`–`GND` |
| Bougez chaque fil pendant la mesure | La valeur ne change pas | Soudure froide ou brin cassé : refaites-la |

Alimentez ensuite d'abord par USB, vérifiez qu'il rejoint le réseau et que `contact` change, et seulement après passez au 12 V.

## Problèmes fréquents

| Symptôme | Cause probable | Solution |
|---|---|---|
| Marche sur l'établi, scintille ou décroche dans le boîtier | Soudure froide ou fil cassé à la soudure | Test du fil bougé ci-dessus ; refaites la soudure avec du flux ; passez les fils de terrain sur borniers à vis |
| `contact` bloqué sur *ouvert* | Soudure ouverte sur `D2` ou sur le câble du reed | Continuité `D2`–`GND` avec l'aimant devant |
| `contact` bloqué sur *fermé* | Pont `D2`–`GND`, ou résidus de flux | Inspectez à la loupe, nettoyez à l'alcool |
| Le XIAO ne s'allume pas, le buck chauffe | Pont `5V`–`GND` | Retirez l'étain avec du flux et de la tresse à dessouder |
| Une pastille du XIAO s'est décollée | Trop chaud, trop long, ou fil arraché | Baissez la température et utilisez des barrettes. Si `D2` est perdue, câblez le reed sur `D1` (GPIO1) et changez `REED_PIN` dans le sketch, ou `pin` dans le YAML ESPHome |
| La soudure fait une boule sur la pastille | Pastille pas chauffée, l'étain a fondu seulement sur la panne | Chauffez pastille et broche ensemble, puis amenez l'étain |
| L'étain n'accroche pas, la panne est noire | Panne oxydée | Nettoyez à la laine de laiton, étamez la panne ; laissez-la étamée au repos |
| La portée Zigbee a baissé | Chaleur ou étain près du connecteur U.FL | Éloignez le fer du coin de l'antenne ; réenfichez la prise U.FL |

Toujours bloqué ? Voir [Dépannage](05-troubleshooting.md).
