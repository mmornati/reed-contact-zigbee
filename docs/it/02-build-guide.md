# 2. Costruzione passo passo

🌐 [English](../02-build-guide.md) · [Français](../fr/02-build-guide.md) · **Italiano**

Segui questi passi in ordine. Provi ogni parte al banco prima che finisca in garage.

## Passo 1: flash e abbinamento al banco (solo USB)

1. Flasha il firmware via USB-C. Vedi [Firmware](03-firmware.md).
2. Installa il converter Z2M e abbina la scheda. Vedi [Zigbee2MQTT e HA](04-zigbee2mqtt-and-home-assistant.md).
3. Collega il contatto reed a `D2` / `GND` con fili corti. Avvicina e allontana il magnete e controlla che Z2M mostri `contact: true` (magnete vicino) e `contact: false` (magnete lontano).

Ora hai un sensore funzionante prima di saldare qualsiasi cosa in modo definitivo.

## Passo 2: regola il convertitore buck a 5,0 V

1. Collega `IN+`/`IN−` dell'LM2596 all'alimentazione a 12 V. **Ancora niente sull'uscita.**
2. Metti un multimetro su `OUT+`/`OUT−`.
3. Gira il trimmer blu (di solito servono molti giri, in senso antiorario la tensione scende) finché non leggi **5,0–5,1 V**.
4. Togli l'alimentazione. Metti una goccia di smalto per unghie o di colla a caldo sul trimmer, così non può spostarsi.

> Non collegare mai lo XIAO prima di aver misurato l'uscita del buck. Questi moduli spesso arrivano regolati tra 12 e 35 V in uscita.

## Passo 3: prepara il contenitore

1. Pianifica la disposizione nella scatola IP67: buck da un lato, XIAO dall'altro, antenna lungo il coperchio o una parete laterale, lontana dall'induttanza del buck.
2. Fora per **2 pressacavi** sul **fondo** della scatola: uno per l'ingresso a 12 V, uno per il cavo del reed. Con i pressacavi in basso l'acqua non può entrare.
3. Monta le schede sulla piastra di fondo forata con distanziali in nylon M2/M3, oppure con biadesivo in schiuma per lo XIAO.
4. Attacca l'antenna U.FL all'interno del coperchio o di una parete laterale. La plastica ABS lascia passare lo Zigbee. Non mettere la scatola **dentro** il carter metallico del motore, né appoggiata a un telaio metallico della porta.

## Passo 4: cabla la scatola

Segui lo [schema di collegamento](01-hardware-and-wiring.md#schema-di-collegamento).

1. Salda la resistenza da 10 kΩ tra `D2` e `3V3`, e il condensatore da 100 nF tra `D2` e `GND`, sui pin dello XIAO (o su un pezzetto di millefori).
2. `OUT+` del buck → `5V` dello XIAO, `OUT−` del buck → `GND` dello XIAO. Saldali, non fidarti di jumper Dupont volanti in un garage.
3. Monta il morsetto a vite o il Wago per il cavo del reed (`D2`, `GND`).
4. Monta il morsetto a vite o il Wago per l'ingresso a 12 V, con il fusibile da 1 A facoltativo sul `+`.
5. Test al banco: alimenta la scatola a 12 V. Il LED dello XIAO lampeggia lentamente mentre si ricollega alla rete, poi si spegne. Il reed funziona ancora in Z2M.

> Non collegare lo XIAO all'USB mentre la scatola è alimentata a 12 V. I 5 V dell'USB e i 5 V del buck finiscono sullo stesso pin. Se devi riflashare, stacca prima i 12 V.

## Passo 5: monta il contatto reed sulla porta

L'obiettivo è rilevare **solo la chiusura completa**, così una porta che si ferma qualche centimetro prima conta comunque come "non chiusa".

1. Chiudi completamente la porta.
2. Scegli un punto in cui porta e telaio sono vicini **solo** alla fine della corsa:
   - **Porta sezionale o basculante:** sul binario laterale vicino al pavimento, oppure sul telaio in corrispondenza del bordo superiore della porta.
   - **Serranda avvolgibile:** l'ultima stecca a livello del pavimento, con il magnete sull'ultima stecca e il reed sulla guida.
3. Fissa il reed (con il cavo) sul **telaio fisso**. Fissa il **magnete** sulla **porta mobile**, allineato al reed.
4. Tieni la distanza sotto i 15 mm circa. Prova con il multimetro o guarda Z2M: apri la porta di 3–5 cm e lo stato deve già passare a `contact: false`. Se non succede, sposta i pezzi per allinearli con più precisione o riduci la distanza.
5. Porta il cavo fino alla scatola in una guaina o con delle clip, lontano dal cavo di alimentazione del motore.

## Passo 6: collega alimentazione e apriporta

1. **Abbassa il magnetotermico.**
2. Prendi i 12 V dai morsetti **costanti** `+12V`/`GND` dello YAVIS. Porta una coppia di fili alla scatola del sensore e una allo ZB-SW01 (oppure collegali in cascata dalla scatola).
3. `COM`/`NO` dello ZB-SW01 → morsetti del pulsante del motore, in parallelo al pulsante a muro (salta se l'hai già fatto).
4. Chiudi tutti i pressacavi e il coperchio.
5. Rialza il magnetotermico.

## Passo 7: controlli finali

| Test | Risultato atteso |
|---|---|
| Porta completamente chiusa | In HA `Garage door` = **Chiusa** |
| Apri la porta | HA = **Aperta** entro circa 1 s |
| Ferma la porta 5 cm prima della chiusura | HA resta su **Aperta** |
| `cover.close_cover` da HA | La porta si chiude, nessuna notifica |
| Blocca la porta (o fermala) durante una chiusura da HA | Notifica dopo 90 s: "non ancora completamente chiusa" |
| Lasciala aperta per 10 min | Notifica "lasciata aperta" |
| Togli i 12 V per 20 min | Notifica "sensore offline" |
| Mappa di Z2M | Il sensore compare come **router** |
