# WordChecker

## Introduzione

Questa è stata la mia sottomissione finale per il progetto di *Algoritmi e Strutture Dati* dell'anno accademico 2021/2022 al Politecnico di Milano.
Il progetto prevedeva di creare un programma in C11 che alla sua base controllasse la correttezza dei caratteri di una parola inserita dall'utente rispetto ad una parola di riferimento, a cui mi riferirò spesso con il nome di *password*.

## Specifiche del progetto

Secondo le specifiche del progetto, il programma avrebbe dovuto ricevere dall'utente un intero, rappresentante la lunghezza della parola, un numero indefinito di parole e poi una serie di *tentativi*. Una volta decise le parole del dizionario e la password, infatti, l'utente proverà a indovinare la password inserendo parole che sono state inserite precedentemente nel dizionario.
Ogni tentativo produce una stringa di caratteri che specifica la correttezza di un dato carattere, in maniera simile al gioco da tavolo *Mastermind*:

* Se il carattere è al posto giusto, viene stampato un `+`
* Se il carattere è nella pasword ma si trova nel posto sbagliato nella parola del tentativo, viene stampato un `|` (detto *pipe*)
* Se il carattere non è in alcun posto nella password, viene stampato un `/` (detto *slash*)

Inoltre, ad ogni tentativo, le parole giocate restringono il campo di ricerca dell'utente poiché le parole che rispettano i vincoli finora riscontrati dal giocatore vengono salvate in una lista di parole *filtrate*, che l'utente può richiamare a se con un comando.

Infine, l'utente ha la possibilità di inserire nuove parole nel dizionario grazie ad un comando. Se sono già state giocate delle parole precedentemente, le parole che vengono aggiunte al dizionario che rispettano i vincoli riscontrati fino a quel momento della partita vengono anche aggiunte alle parole filtrate.

## Soluzione proposta

Per risolvere questo problema, ho utilizzato tre strutture dati:

* Un **Red-Black Tree** (a cui mi riferirò come RBT da ora in poi) per contenere l'intero dizionario delle parole
* Una **Linked List** per contenere la lista delle parole filtrate
* Una **Linked List** per contenere le varie restrizioni trovate durante le partite

All'inizio della partita e quando il giocatore chiama il comando `+inserisci_inizio`, viene chiamata una funzione che inserisce le nuove parole nel RBT e, nel caso in cui il giocatore sia nel mezzo di una partita, la parola corrente viene confrontata con la lista delle restrizioni per controllare che rispetti i vincoli riscontrati finora. Nel caso non sia così, la funzione ritorna senza aggiungere la parola alla lista delle parole filtrate.

Un vincolo viene salvato nella sua lista compilando una serie di campi contenuti nel nodo della lista stessa:

* **Carattere**: il carattere a cui si riferisce il vincolo (tipo `char`)
* **Indice**: l'indice in cui deve o non deve comparire il carattere (tipo `int`)
* **Corretto**: uno pseudo-booleano che indica se il carattere deve o non deve comparire all'indice presente nel campo `indice` (tipo `int`)
* **Contatore**: contiene il numero delle occorrenze del carattere nella password (tipo `int`)
* **Esatto**: uno pseudo-booleano che indica se il numero espresso in `contatore` è il numero esatto di caratteri contenuti nella password o indica un limite inferiore (*la password contiene almeno `n` caratteri*, tipo `int`)

Il controllo della correttezza della parola viene fatto controllando innanzitutto la quantità di caratteri di ciascun tipo contenuti nella password e nella parola del tentativo corrente. Questo processo viene eseguito attraverso una funzione che conta in un array apposito il numero di caratteri corrispondente ad un certo indice, che segue la progressione ASCII dei caratteri ammessi dalle specifiche, cioè le lettere maiuscole e minuscole, le cifre ed i due caratteri speciali `-` e `_`.

Nel caso in cui il numero di caratteri di un certo tipo nella password sia maggiore dello stesso numero nella parola del tentativo corrente, sicuramente non viene scoperto il numero esatto di occorrenze corrette di tale carattere, perciò gli unici controlli che vengono fatti su quel carattere sono sulla correttezza dell'indice: se è al posto giusto, lo slot corrispondente nella *guida* di correttezza viene riempito da `+`, mentre altrimenti viene riempito da `|`.

Al contrario, se nella parola del tentativo corrente ci sono più caratteri di un certo tipo rispetto a quelli nella password, sicuramente almeno uno di essi sarà assente poiché di troppo e dovrà essere colmato da `/`. Per eseguire questo controllo in modo efficiente, ho utilizzato la funzione `strchr`, presente nella libreria standard C `string.h`, che ritorna il puntatore alla prima occorrenza di un carattere specificato in una stringa.

Il primo controllo eseguito dalla funzione in questo caso è quello dei caratteri corretti (`+`). In questo caso, due puntatori alle stringhe della password e della parola corrente vengono *sincronizzati*, incrementandoli nel caso uno dei due rimanga *indietro*, ossia ad un indice minore dell'altro. Così facendo, posso essere sicuro di trovare tutti i caratteri che hanno indice uguale e segnalarli così con `+`.

Dopodiché, finché il numero di caratteri corretti nella password è maggiore di zero, continuo a scorrere la parola del tentativo corrente dall'inizio e segno con `|` tutti i caratteri uguali che non sono ancora stati segnalati precedentemente.

Una volta che il numero di caratteri corretti nella password è zero, posso trovare tutti i caratteri uguali rimanenti e segnalarli con `/`.

Infine, discutiamo la gestione della lista delle parole filtrate.

Quando viene giocata la prima partita, la lista delle parole filtrate è necessariamente vuota, perciò il primo controllo viene fatto su tutto l'albero contenente il dizionario completo delle parole. Man mano che questo viene controllato, le parole che rispettano i vincoli trovati dal primo tentativo vengono aggiunte alla lista delle parole filtrate in ordine lessicografico. Dopodiché, la lista viene svuotata progressivamente quando vengono trovati nuovi vincoli.

Se viene chiamato il comando `+stampa_filtrate`, che stampa tutta la lista delle parole filtrate a schermo, prima che la lista delle filtrate sia stata creata, essa viene riempita facendo un *inorder tree walk* inverso: dato che un RBT contiene i valori in esso salvati in ordine crescente da sinistra a destra e il metodo più veloce di riempire una lista è dalla testa con un *push*, l'albero viene *camminato* dal nodo massimo al nodo minimo e le parole vengono aggiunte alla lista sempre dalla testa, connettendo il prossimo elemento in testa di quello precedente. In questo modo si ottiene una lista ordinata in ordine lessicografico e creata nel minor tempo possibile, che è `O(n)`.
