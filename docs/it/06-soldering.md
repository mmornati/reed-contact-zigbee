# 6. Saldare lo XIAO

🌐 [English](../06-soldering.md) · [Français](../fr/06-soldering.md) · **Italiano**

Lo XIAO ESP32-C6 è piccolo: i suoi pad sono a 2,54 mm l'uno dall'altro e `5V`, `GND` e `3V3` sono uno accanto all'altro. Quasi tutti i problemi del tipo "funziona, poi non più" vengono da una saldatura difettosa su quei pad o da un filo che si spezza dove esce dalla saldatura. Questa guida propone un montaggio che evita entrambi.

## In breve

1. **Non saldare i fili di campo sullo XIAO.** Salda gli header a pettine sullo XIAO, montalo su una piccola millefori e porta il cavo del reed e i 5 V su dei **morsetti a vite** della millefori.
2. **Flussante, punta pulita e 2–3 secondi per saldatura.** Se lo stagno non scorre in quel tempo, fermati, aggiungi flussante e riprova. Non continuare a scaldare.
3. **Misura prima di alimentare.** Controlla col multimetro che non ci siano corti tra `5V`, `3V3` e `GND` ogni volta che saldi qualcosa.

## Attrezzi e materiali

| Cosa | Consigliato | Perché |
|---|---|---|
| Saldatore | A temperatura controllata, 40–80 W, punta piccola a scalpello (1,5–2 mm) | Una punta conica "ad ago" trattiene troppo poco calore per i pin |
| Temperatura | **330–350 °C** con stagno al piombo, **350–370 °C** senza piombo | Più caldo, il flussante brucia prima che lo stagno scorra e i pad possono staccarsi |
| Stagno | 0,5–0,8 mm, con anima di flussante. Il 63/37 al piombo è il più facile | Uno stagno spesso crea ponti su un passo di 2,54 mm |
| Flussante | Penna o gel "no-clean" | Il rimedio principale per saldature opache e granulose |
| Pulizia punta | Lana di ottone | Una spugna bagnata raffredda la punta e la consuma prima |
| Supporto | Terza mano, o una breadboard per tenere dritti gli header | Due mani libere, pin ad angolo retto |
| Millefori | Un pezzetto, passo 2,54 mm | Ospita XIAO, resistenza, condensatore e morsetti |
| Morsetti a vite | 2 poli, passo 5,08 mm o 3,5 mm, ×2 | Cavo del reed e ingresso 5 V |
| Multimetro | Con prova di continuità (beep) | Tutti i controlli qui sotto |
| Alcol isopropilico + vecchio spazzolino | 90 % o più | Pulisce i residui di flussante tra i pad |

## I pin dello XIAO che usi

Vista dall'alto, USB-C in alto:

```
            ┌──[ USB-C ]──┐
   D0  ●    │             │    ●  5V    ← OUT+ del buck
   D1  ●    │   XIAO      │    ●  GND   ← OUT− del buck, COM del reed, 100 nF
   D2  ●    │  ESP32-C6   │    ●  3V3   ← 10 kΩ
   D3  ●    │             │    ●  D10
   D4  ●    │             │    ●  D9
   D5  ●    │             │    ●  D8
   D6  ●    │     U.FL ◯  │    ●  D7
            └─────────────┘
   D2 ← NO del reed, 10 kΩ, 100 nF
```

Servono solo quattro pin: `D2`, `5V`, `GND`, `3V3`. `5V`/`GND`/`3V3` sono tre pad vicini, quindi un ponte tra loro è l'errore classico. Lascia stare i pad della batteria sul retro.

## Montaggio consigliato: lo XIAO su una millefori

```
   millefori (vista dall'alto)
   ┌───────────────────────────────────────────────┐
   │   D2 ●─────┬───────────[10 kΩ]────────● 3V3   │
   │            │                                  │
   │          [100 nF]                             │
   │            │                                  │
   │  GND ●─────┴──────────────────────────● GND   │
   │                                               │
   │  [morsetto: D2 | GND]  ← cavo del reed        │
   │  [morsetto: 5V | GND]  ← uscita del buck      │
   └───────────────────────────────────────────────┘
      XIAO innestato sugli header al centro
```

`D2` e `3V3` sono sui lati opposti dello XIAO: per questo la resistenza va sulla millefori e non di traverso sulla scheda.

## Passo per passo

Scollega USB e 12 V prima di iniziare. Non saldare mai una scheda alimentata.

### 1. Header sullo XIAO

1. Infila due header maschi da 7 pin in una breadboard, lato lungo verso il basso, distanziati quanto la larghezza dello XIAO. La breadboard li tiene dritti.
2. Appoggia lo XIAO sopra, componenti verso l'alto.
3. Metti flussante sui pad. Salda **un pin d'angolo** per lato, poi controlla che la scheda sia ben in piano. Se no, riscalda quel pin premendo la scheda.
4. Salda il resto. Appoggia la punta **su pad e pin insieme**, aspetta 1 secondo, porta lo stagno nella saldatura (non sulla punta), togli lo stagno e poi il saldatore. 2–3 s in tutto.
5. Una buona saldatura è un piccolo cono lucido che avvolge il pin e copre il pad. Una pallina appoggiata sopra, o una superficie grigia e opaca, è una saldatura fredda: aggiungi flussante e riscalda.

> Se vuoi saltare questo passo, Seeed vende anche lo XIAO con gli header già saldati.

### 2. Componenti sulla millefori

1. Salda degli header femmina sulla millefori per lo XIAO (così resta estraibile per riflasharlo), oppure salda direttamente i suoi header.
2. Monta la **10 kΩ** tra la fila `D2` e la fila `3V3`, e il **100 nF** tra la fila `D2` e una fila `GND`. Piega i reofori in piano sul retro e usali come piste.
3. Monta i due morsetti e collegali a `D2`/`GND` e `5V`/`GND` con i reofori dei componenti o con spezzoni di filo rigido.
4. Lascia almeno un foro tra le piste `5V` e `GND`. Taglia i reofori in eccesso perché non possano toccarsi in seguito.

### 3. Fili nei morsetti

1. Usa filo **flessibile** (multifilare) per il cavo del reed e l'alimentazione a 5 V. Il filo rigido si spezza dopo qualche mese di vibrazioni.
2. Spela 6–7 mm. Usa dei puntalini, oppure attorciglia i fili e stringili **senza** stagnarli: un filo stagnato in un morsetto a vite si allenta col tempo.
3. Se devi saldare un filo ai terminali del reed, infila prima una **guaina termorestringente**, stagna entrambi i capi, uniscili, poi restringi la guaina sulla saldatura. Aggiungi un punto di colla a caldo o una fascetta, così il cavo tira sulla fascetta e non sulla saldatura.

### 4. Pulizia

Strofina le saldature con alcol isopropilico e uno spazzolino. Residui di flussante tra `D2` e `GND` possono far passare un po' di corrente in un garage umido e rendere instabile l'ingresso.

## Controlli prima di alimentare

Col multimetro, senza alimentazione, XIAO montato:

| Misura tra | Atteso | Se no |
|---|---|---|
| `5V` e `GND` | Nessun beep (nessun corto) | Cerca un ponte sui pad di destra o sul morsetto |
| `3V3` e `GND` | Nessun beep | Idem |
| `D2` e `3V3` | Circa **10 kΩ** (un po' meno è normale con lo XIAO montato) | Saldatura della resistenza aperta, o fila sbagliata |
| `D2` e `GND`, magnete davanti al reed | Beep (circa 0 Ω) | Reed cablato su NC invece di NO, o saldatura rotta sul cavo |
| `D2` e `GND`, magnete lontano | Nessun beep | Coppia di fili del reed sbagliata, o ponte `D2`–`GND` |
| Muovi ogni filo durante la misura | Il valore non cambia | Saldatura fredda o filo spezzato: rifalla |

Poi alimenta prima via USB, controlla che si colleghi alla rete e che `contact` cambi, e solo dopo passa ai 12 V.

## Problemi frequenti

| Sintomo | Causa probabile | Soluzione |
|---|---|---|
| Funziona al banco, sfarfalla o si scollega nella scatola | Saldatura fredda o filo spezzato alla saldatura | Prova del filo mosso qui sopra; rifai la saldatura con flussante; sposta i fili di campo su morsetti a vite |
| `contact` bloccato su *aperto* | Saldatura aperta su `D2` o sul cavo del reed | Continuità `D2`–`GND` con il magnete davanti |
| `contact` bloccato su *chiuso* | Ponte `D2`–`GND`, o residui di flussante | Controlla con una lente, pulisci con l'alcol |
| Lo XIAO non si accende, il buck scalda | Ponte `5V`–`GND` | Togli lo stagno con flussante e treccia dissaldante |
| Si è staccato un pad dello XIAO | Troppo caldo, troppo a lungo, o filo strappato | Abbassa la temperatura e usa gli header. Se hai perso `D2`, collega il reed a `D1` (GPIO1) e cambia `REED_PIN` nello sketch, o `pin` nello YAML di ESPHome |
| La saldatura fa una pallina sul pad | Pad non scaldato, lo stagno si è fuso solo sulla punta | Scalda pad e pin insieme, poi porta lo stagno |
| Lo stagno non attacca, la punta è nera | Punta ossidata | Pulisci con la lana di ottone, stagna la punta; lasciala stagnata a riposo |
| La portata Zigbee è peggiorata | Calore o stagno vicino al connettore U.FL | Tieni il saldatore lontano dall'angolo dell'antenna; reinnesta il connettore U.FL |

Ancora bloccato? Vedi [Risoluzione dei problemi](05-troubleshooting.md).
