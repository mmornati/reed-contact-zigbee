# 1. Hardware e collegamenti

🌐 [English](../01-hardware-and-wiring.md) · [Français](../fr/01-hardware-and-wiring.md) · **Italiano**

> ⚠️ L'alimentatore YAVIS è alimentato da rete (90–260 V AC). Abbassa l'interruttore magnetotermico prima di toccare i suoi morsetti di ingresso. Tutto il resto di questo progetto è a bassa tensione (12 V / 5 V / 3,3 V).

## Bilancio dei consumi

| Carico | Tipico | Picco |
|---|---|---|
| XIAO ESP32-C6, router Zigbee (radio sempre accesa) | ~30 mA @ 5 V | ~120 mA @ 5 V |
| Relè ZB-SW01 | ~30 mA @ 12 V | ~80 mA @ 12 V (bobina eccitata) |
| Kit controllo accessi (tastierino + elettromagnete da 180 kg) | ~500 mA @ 12 V | ~600 mA |

Il totale è ben sotto 1 A. L'alimentatore da 5 A ha margine in abbondanza.

## Risposte alle domande sull'alimentazione

- **ZB-SW01 direttamente a 12 V?** Sì. Accetta 5–32 V DC sui morsetti di alimentazione. Controlla l'etichetta del tuo esemplare (alcuni lotti riportano "DC 7–32 V" oppure accettano anche AC) e rispetta la polarità +/−.
- **XIAO direttamente a 12 V?** **No.** Il pin 5 V dello XIAO deve restare intorno ai 5 V, quindi il buck LM2596 è indispensabile. Porta i 5,0 V del buck sul pin **5V** dello XIAO, non su 3V3. Il regolatore interno dello XIAO genera poi dei 3,3 V puliti.
- **Quali morsetti dello YAVIS?** Gli alimentatori per controllo accessi hanno un'uscita DC **costante** (`+12V`/`DC+` e `GND`/`DC−`) e un'uscita **serratura commutata** (`NO`/`COM`/`NC`, `PUSH`, temporizzatore). Usa solo i morsetti DC costanti. L'uscita serratura si spegne ogni volta che la serratura si sblocca e farebbe riavviare il sensore.

### Mi serve la batteria?

No. Il sensore funziona da rete come router Zigbee, che è la configurazione migliore: nessuna batteria da ricaricare, e rinforza la mesh. Una LiPo sui pad batteria dello XIAO servirebbe solo come backup durante un blackout, ma durante un blackout si fermano anche il motore della porta e l'apriporta, e le celle LiPo non amano un garage freddo d'inverno e caldo d'estate. Tienila per un altro progetto.

## Schema di collegamento

Lo stesso cablaggio va bene per entrambi i firmware: lo sketch Arduino e lo [YAML di ESPHome](03-firmware.md) (opzione C) usano gli stessi pin (reed su D2 / GPIO2, commutatore d'antenna su GPIO3 / GPIO14, BOOT su GPIO9, LED su GPIO15).

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

### Tabella dei collegamenti

| Da | A | Filo |
|---|---|---|
| YAVIS `+12V` (costante) | Fusibile 1 A → LM2596 `IN+` | rosso, 0,5 mm² |
| YAVIS `GND` | LM2596 `IN−` | nero, 0,5 mm² |
| YAVIS `+12V` (costante) | ZB-SW01 `+` / `DC+` | rosso |
| YAVIS `GND` | ZB-SW01 `−` / `DC−` | nero |
| LM2596 `OUT+` (**5,0 V**) | XIAO `5V` | rosso |
| LM2596 `OUT−` | XIAO `GND` | nero |
| Reed `COM` | XIAO `GND` | conduttore 1 del cavo |
| Reed `NO` | XIAO `D2` (GPIO2) | conduttore 2 del cavo |
| Resistenza da 10 kΩ | tra XIAO `D2` e `3V3` | lato XIAO |
| Condensatore da 100 nF | tra XIAO `D2` e `GND` | lato XIAO |
| ZB-SW01 `COM` + `NO` | morsetti pulsante del motore | come da manuale del motore |
| Antenna U.FL | connettore antenna dello XIAO | |

### Perché NO e non NC?

Con il contatto **NO** del reed, l'interruttore si chiude solo quando il magnete gli sta davanti, cioè quando la porta è completamente chiusa. Lo XIAO legge allora `D2 = LOW`, cioè chiuso. Qualsiasi guasto (porta socchiusa, cavo tagliato, connettore allentato, sensore rotto) dà `D2 = HIGH`, cioè **non chiuso**, così ricevi un avviso invece di un falso "tutto ok".

Verificalo con un multimetro prima del montaggio. Questi reed economici non sempre etichettano i fili allo stesso modo:

1. Metti il multimetro in prova di continuità.
2. Trova la coppia che **suona solo quando il magnete è vicino**. Quelli sono COM + NO.
3. La coppia che suona quando il magnete è **lontano** è COM + NC. Lascia NC scollegato e isolato.

### Perché 10 kΩ + 100 nF?

Il firmware abilita già il pull-up interno dell'ESP32 (circa 45 kΩ). Su qualche metro di cavo che corre vicino a un motore è debole e capta disturbi. Un pull-up esterno da 10 kΩ più 100 nF verso massa rendono il segnale solido. Il firmware fa anche un debounce di 200 ms.

## ZB-SW01 sul motore

- Collega **COM / NO** dello ZB-SW01 all'ingresso pulsante del motore (spesso marcato `START`, `PP`, `P/P`, `PB`, `OPEN` + `COM`/`GND`). Mettilo **in parallelo** al pulsante a muro esistente, così funzionano entrambi.
- Usa la **modalità inching (impulsiva)** se il dispositivo la offre. Lo script di HA lo spegne comunque dopo 800 ms, quindi funziona in entrambi i casi.
- Non collegare mai i contatti dello ZB-SW01 ai morsetti di potenza del motore. È un contatto pulito che simula soltanto la pressione di un pulsante.
