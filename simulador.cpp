#include <bits/stdc++.h>

using namespace std;
/*
Version Timestamp
Simulador de memoria virtual
Sistemas Operativos
Miembros:
Francisco Salgado Guizar A01365047
Gabriel Gomez Novau A01720009
*/


//Structura que tendra los datos de cada proceso
struct proceso {
    //Constructor que inicializa los valores del objeto
    int idp;//id del proceso
    float timestamp;//tiempo de creacion
    proceso(int idt, float timestampt) {
        idp = idt;
        timestamp = timestampt;
    }
    int paginas;//numero de paginas del proceso
    int pagefaultsLRU = 0;//page faults en LRU
    int pagefaultsFIFO = 0;//page fault en fifo
    float turnaroundFIFO = 0;//turnaround en fifo
    float turnaroundLRU = 0;// turnaroun en LRU
};

float tiempo = 0;//variable para ir manejando el tiempo que llevan los procesos FIFO
float tiempo2 = 0;//variable para ir manejando el tiempo que llevan los procesos LRU

int swapsFIFO = 0;//numero de swaps en FIFO
int swapsLRU = 0;//numero de swaps en LRU

vector<proceso> agregados;//vector que guardara los procesos agregados a memoria para luego revisarlos 
vector<proceso> liberados; //vector que guarda a los procesos que se vayan liberando de memoria para luego usar como estadisticas de turnaround y pagefaults
vector<proceso> agregados2;//vector que guardara los procesos agregados a memoria para luego revisarlos 
vector<proceso> liberados2; //vector que guarda a los procesos que se vayan liberando de memoria para luego usar como estadisticas de turnaround y pagefaults

//Funcion que hace el swapping unicamente para procesos recien creados que no quepan en memoria principal
void swap_creados(int pagina, float id, float(&M)[128][4], float(&S)[256][4], int politica, bool created) {
    int cont = 0;// se usara para ir contando las paginas nuevas
    while (pagina > 0) {//mientras que no se acabe de cargar el proceso en swap a la memoria real no se acaba
        float temporal = 999999;// para comparar el timestamp y ver cuales es el first in en memoria real
        int tempoIndice = -1;//para saber que indice es el que tiene el timestamp mas bajo
        for (int i = 0; i < 128; i++) {
            if (M[i][2] < temporal) {//se checa cual es el first in the todas las paginas
                temporal = M[i][2];//temporal para guardar el timestamp mas bajo e ir comparando cada vez
                tempoIndice = i;//para saber cual es el indice con el timestamp mas bajo
            }
        }
        for (int i = 0; i < 256; i++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
            if (S[i][0] == -1) {
                S[i][0] = M[tempoIndice][0];//el id del proceso swapeado
                S[i][1] = M[tempoIndice][1]; //el numero de pagina del proceso
                if (politica == 1) {
                    S[i][2] = tiempo;//el timestamp al momento de swapear
                    tiempo = tiempo + 1;//se incrementa el tiempo por swapear a out
                    swapsFIFO++;
                }
                else {
                    S[i][2] = tiempo2;//el timestamp al momento de swapear
                    tiempo2 = tiempo2 + 1;//se incrementa el tiempo por swapear a out
                    swapsLRU++;
                }

                S[i][3] = M[tempoIndice][3];//se ve cual es la direccion en memoria
                cout << "Pagina " << S[i][1] << " del proceso " << S[i][0] << " swappeada al marco " << i << " del area de swapping" << endl;
                break;
            }
        }
        if (!created) {//se checa si es el proceso es nuevo o ya fue agregado al vector de agregados
            if (politica == 1) {
                proceso temp(id, tiempo);//se crea un temporal objeto proceso para poder pushear el proceso despues a agregados
                temp.paginas = pagina;//se asigna la cantidad de paginas del proceso
                created = true;
                agregados.push_back(temp);//se agrega el nuevo proceso creado
            }
            if(politica == 2){
                proceso temp(id, tiempo2);//se crea un temporal objeto proceso para poder pushear el proceso despues a agregados
                temp.paginas = pagina;//se asigna la cantidad de paginas del proceso
                created = true;
                agregados2.push_back(temp);//se agrega el nuevo proceso creado
            }
        }
        M[tempoIndice][0] = id;//se cambia el id al nuevo proceso
        M[tempoIndice][1] = cont;//se asigna el numero de pagina del proceso
        cont++;
        if (politica == 1) {
            M[tempoIndice][2] = tiempo;//time stamp de la pagina
            tiempo = tiempo + 1;//se modifico algo entonces se agrega 1 segundo
            if (created) {
                agregados.back().pagefaultsFIFO++;//se genero un page fault por eso se agrega al contador
            }
        }
        if(politica == 2)
        {
            M[tempoIndice][2] = tiempo2;//time stap de la pagina
            tiempo2 = tiempo2 +1;//se modifico algo entonces se agrega 1 segundo
            if (created) {
                agregados2.back().pagefaultsLRU++;//se genero un page fault por eso se agrega al contador
            }
        }
        M[tempoIndice][3] = tempoIndice;// se asigna el la direccion de memoria
        pagina--;//se va ir a la siguiente pagina a cambiar
    }
}

void cargar(float bytes, float id, float(&M)[128][4], float(&S)[256][4], int politica) {
    bool creado = false;//Variable que se utiliza si el para agregar al proceso al vector de procesos si se encuentra un espacio para el
    int aux; //Variable que se va a utilizar para contar la cantidad de paginas disponibles en el area de swapping
    int pagina = ceil(bytes / 16); //Variable que calcula la cantidad de marcos de pagina necesarios para el proceso, siempre redondeando para mayor espacio
    int tempPaginas = pagina;//Nos ayuda a recordar la cantidad de paginas que que guardaran en el structure proceso
    int cont = 0;
    for (int i = 0; i < 128; i++) { //Loop que recorre toda la memoria y en caso de encontrar el espacio necesario, guarda el proceso en la memoria
        if (M[i][0] == -1) {//Revisa desde donde se encuentra el espacio vacio mas las paginas necesarias que no se pase de la memoria
            M[i][0] = id;
            M[i][1] = cont;
            cont++;
            if (politica == 1) {
                M[i][2] = tiempo;
                tiempo = tiempo + 1;
            }
            else {
                M[i][2] = tiempo2;
                tiempo2 = tiempo2 +1;
            }
            M[i][3] = i;
            pagina--;
            if (!creado) {
                if (politica == 1) {
                    proceso Nuevo(id, tiempo); //Creacion de un nuevo proceso, con su respectivo ID proveido por el usuario
                    Nuevo.paginas = tempPaginas;
                    agregados.push_back(Nuevo);//se agrega el nuevo proceso creado
                    creado = true;
                }
                else {
                    proceso Nuevo(id, tiempo2); //Creacion de un nuevo proceso, con su respectivo ID proveido por el usuario
                    Nuevo.paginas = tempPaginas;
                    agregados2.push_back(Nuevo);//se agrega el nuevo proceso creado
                    creado = true;
                }
            }
        }
        if (i == 127) {//En caso de que no se pueda meter el proceso, se hace swapping
            //El 1 indica politica de reemplazo FIFO y el 2 LRU
            if (politica == 1) {
                aux = 0;
                for(int a = 0 ; a < 256; a++){//Primero se revisa si se tiene espacio en la memoria de swapping 
                    if(S[a][0] == -1){
                        aux++;
                    }
                }
                if(aux >= pagina){ 
                    swap_creados(pagina, id, M, S, politica, creado);
                }
                else{
                    cout << "FIFO: No se ha encontrado espacio en la memoria de swapping para meter al proceso\nNo se procesara" << endl;
                }
            }
            else {
                aux = 0;
                for(int a = 0 ; a < 256; a++){//Primero se revisa si se tiene espacio en la memoria de swapping 
                    if(S[a][0] == -1){
                        aux++;
                    }
                }
                if(aux >= pagina){ 
                    swap_creados(pagina, id, M, S, politica, creado);
                }
                else{
                    cout << "LRU: No se ha encontrado espacio en la memoria de swapping para meter al proceso\nNo se procesara" << endl;
                }
            }
            pagina = 0;
        }
        if (pagina == 0) {
            break;
        }
    }
}

void FIFO(int direccion, int id, int modificacion, float(&M)[128][4], float(&S)[256][4]) {
    int pagina = floor(direccion / 16);//pagina es igual a la direccion entre 16 bytes que es el tamano de las paginas
    int real = 0;//queremos ver cual es la direccion real
    int menor = 999999;//se usara para estar viendo cual es el timestamp mas bajo
    int ind = 0;//se usara para saber el indice del proceso a cambiar
    cout << "FIFO: Obtener la direccion real correspondiente a la direccion virtual " << direccion << " del proceso " << id << endl;
    for (int k = 0; k < 128; k++) {//se recorre toda la memoria real
        if (M[k][0] == id && M[k][1] == pagina) {//este if es para saber si es el el proceso buscado y el la pagina correcta
            real = (direccion % 16) + k * 16;//la direccion real del proceso a swapear
            if (modificacion == 1) {//es para saber si el proceso se va a modificar
                cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
            }
            cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
            tiempo = tiempo + 0.1;//se modifica asi que se anade el tiempo a todo el programa
            break;
        }

        if (k == 127) {//si se llega al final de la memoria real se entra aqui
            for (int i = 0; i < agregados.size(); i++) {//se busca en todos los procesos agregados en el sistema
                if (agregados[i].idp == id) {// si se encuentra se id se agrega un page fault
                    agregados[i].pagefaultsFIFO++;
                }
            }
            for (int i = 0; i < 256; i++) {//se busca en la memoria virtual
                if (S[i][0] == id && S[i][1] == pagina) {//se busca si se encuentra el proceso y la pagina a swapear en la memoria virtual
                    for (int j = 0; j < 128; j++) {//se busca en la memoria real cual es el que tiene el timestamp mas chico
                        if (M[j][2] < menor) {
                            menor = M[j][2];
                            ind = j;
                        }
                        for (int x = 0; x < 256; x++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
                            if (S[x][0] == -1) {
                                S[x][0] = M[ind][0];//el id del proceso swapeado
                                S[x][1] = M[ind][1]; //el numero de pagina del proceso
                                S[x][2] = tiempo;//el timestamp al momento de swapear
                                tiempo = tiempo + 1;//se incrementa el tiempo por swapear a out
                                S[x][3] = M[ind][3];//se ve cual es la direccion en memoria
                                //se swapean el proceso nuevo a memoria real
                                M[ind][0] = S[i][0];
                                M[ind][1] = S[i][1];
                                M[ind][2] = tiempo;
                                tiempo = tiempo + 1;
                                M[ind][3] = S[i][3];
                                //se elimina el proceso de swapeado de memoria virtual
                                S[i][0] = -1;
                                S[i][1] = -1;
                                S[i][2] = -1;
                                S[i][3] = -1;

                                cout << "Pagina " << S[x][1] << " del proceso " << S[x][0] << " swapeada al marco " << x << " del area de swapping" << endl;
                                cout << "Se localizo la pagina " << M[ind][1] << " que estaba en la posicion " << i << " de swapping y cargo al marco " << ind << endl;
                                real = (direccion % 16) + k * 16;//para saber la direccion real en memoria
                                if (modificacion == 1) {//saber si se modificara o no
                                    cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
                                }
                                cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
                                tiempo = tiempo + 0.1;
                                swapsFIFO++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void LRU(int direccion, int id, int modificacion, float(&M)[128][4], float(&S)[256][4]) {
    int pagina = floor(direccion / 16);//pagina es igual a la direccion entre 16 bytes que es el tamano de las paginas
    int real = 0;//queremos ver cual es la direccion real
    int menor = 999999;//se usara para estar viendo cual es el timestamp mas bajo
    int ind = 0;//se usara para saber el indice del proceso a cambiar
    cout << "LRU: Obtener la direccion real correspondiente a la direccion virtual " << direccion << " del proceso " << id << endl;
    for (int k = 0; k < 128; k++) {//se recorre toda la memoria real
        if (M[k][0] == id && M[k][1] == pagina) {//este if es para saber si es el el proceso buscado y el la pagina correcta
            real = (direccion % 16) + k * 16;//la direccion real del proceso a swapear
            if (modificacion == 1) {//es para saber si el proceso se va a modificar
                cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
            }
            cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
            M[k][2] = tiempo2;//como se uso se cambia el timestamp porque en LRU se ocupa la ultima vez usada
            tiempo2 = tiempo2 + 0.1;//se modifica asi que se anade el tiempo a todo el programa
            break;
        }

        if (k == 127) {//si se llega al final de la memoria real se entra aqui
            for (int i = 0; i < agregados2.size(); i++) {//se busca en todos los procesos agregados en el sistema
                if (agregados2[i].idp == id) {// si se encuentra se id se agrega un page fault
                    agregados2[i].pagefaultsLRU++;
                }
            }
            for (int i = 0; i < 256; i++) {//se busca en toda la memoria virtual
                if (S[i][0] == id && S[i][1] == pagina) {//se busca el proceso id y la pagina correcta
                    for (int j = 0; j < 128; j++) {//se busca cual es el timestamp mas viejo o chico en memoria real
                        if (M[j][2] < menor) {
                            menor = M[j][2];
                            ind = j;
                        }
                        for (int x = 0; x < 256; x++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
                            if (S[x][0] == -1) {
                                S[x][0] = M[ind][0];//el id del proceso swapeado
                                S[x][1] = M[ind][1]; //el numero de pagina del proceso
                                S[x][2] = tiempo2;//el timestamp al momento de swapear
                                tiempo2 = tiempo2 + 1;//se incrementa el tiempo por swapear a out
                                S[x][3] = M[ind][3];//se ve cual es la direccion en memoria

                                M[ind][0] = S[i][0];//el id del proceso swapeado
                                M[ind][1] = S[i][1]; //el numero de pagina del proceso
                                M[ind][2] = tiempo2;//el timestamp al momento de swapear
                                tiempo2 = tiempo2 + 1;//se incrementa el tiempo por swapear a out
                                M[ind][3] = S[i][3];//se ve cual es la direccion en memoria
                                //se vacia la pagina de la memoria virtual
                                S[i][0] = -1;
                                S[i][1] = -1;
                                S[i][2] = -1;
                                S[i][3] = -1;

                                cout << "Pagina " << S[x][1] << " del proceso " << S[x][0] << " swapeada al marco " << x << " del area de swapping" << endl;
                                cout << "Se localizo la pagina " << M[ind][1] << " que estaba en la posicion " << i << " de swapping y cargo al marco " << ind << endl;
                                real = (direccion % 16) + k * 16;//se busca la direccion real de proceso
                                if (modificacion == 1) {//para saber si se modifico algo y desplegar que paso
                                    cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
                                }
                                cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
                                tiempo2 = tiempo2 + 0.1;
                                swapsLRU++;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}

void borrarProceso(int id, float(&M)[128][4], float(&S)[256][4], int politica) {
    if(politica == 1){
        cout << "FIFO: Liberar los marcos de paginas ocupados por el proceso: " << id << endl;
    }
    else{
        cout << "LRU: Liberar los marcos de paginas ocupados por el proceso: " << id << endl;
    }
    vector<int> vect;//se hace un vector para guardar los indices de la paginas borradas
    for (int i = 0; i < 128; i++) {//se ve toda la memoria real para saber cuales son las paginas que se tienen que borrar que tienen el id del proceso a borrar
        if (M[i][0] == id) {
            M[i][0] = -1;
            M[i][1] = -1;
            M[i][2] = -1;
            M[i][3] = -1;
            vect.push_back(i);//se pushea el indice borrado en la memoria real
            if (politica == 1) {
                tiempo = tiempo + 0.1;//se borro una pagina asi que se modifica el tiempo sistema total
            }
            else {
                tiempo2 = tiempo2 + 0.1;//se borro una pagina asi que se modifica el tiempo sistema total
            }
        }
    }
    if (vect.size() == 1) {//si nadamas se saco una pagina se imprime cual es
        cout << "Se liberan los marcos de pagina de memoria real " << vect[0] << endl;
    }
    else if (vect.size() > 1) {//si es mas de una pagina borrada se va aqui a imprimir cuales eran
        cout << "Se liberan los marcos de pagina de memoria real ";
        for (int i = 0; i < vect.size(); i++) {//un ciclo viendo cuantas paginas fueron borradas
            cout << vect[i] << "-";//se imprime donde empieza el rango de paginas borradas
            while (vect[i] == vect[i + 1] - 1 && i< vect.size()-1) {//si la pagina que sigue es el mimso numero si se le resta uno es que es consiguiente
                i++;
            }
            if (i != vect.size() - 1) {//esto si todavia no es la ultima vuelta se imprime la parte final del rango
                cout << vect[i] << ", " << endl;
            }
            else if(i == vect.size()-1){//esta parte es para imprimir si es la ultima pagina y no es consiguiente de las anteriores
                cout << vect[i] << endl;
            }
        }
    }
    vector<int> vect2;
    for (int i = 0; i < 256; i++) {//se ve toda la memoria virtual para saber cuales son las paginas que se tienen que borrar que tienen el id del proceso a borrar
        if (S[i][0] == id) {
            S[i][0] = -1;
            S[i][1] = -1;
            S[i][2] = -1;
            S[i][3] = -1;
            vect2.push_back(i);//se pushea el indice borrado en la memoria virtual
            if (politica == 1) {
                tiempo = tiempo + 0.1;//se borro una pagina asi que se modifica el tiempo sistema total
            }
            else {
                tiempo2 = tiempo2 + 0.1;//se borro una pagina asi que se modifica el tiempo sistema total
            }
        }
    }
    if (vect2.size() == 1) {//si nadamas se saco una pagina se imprime cual es
        cout << "Se liberan los marcos de pagina del area de swapping " << vect2[0] << endl;;
    }
    else if (vect2.size() > 1) {//si es mas de una pagina borrada se va aqui a imprimir cuales eran
        cout << "Se liberan los marcos de pagina del area de swapping ";
        for (int i = 0; i < vect2.size(); i++) {//un ciclo viendo cuantas paginas fueron borradas
            cout << vect2[i] << "-";//se imprime donde empieza el rango de paginas borradas
            while (vect2[i] == vect2[i + 1] - 1 && i < vect2.size()-1) {//si la pagina que sigue es el mimso numero si se le resta uno es que es consiguiente
                i++;
            }
            if (i != vect2.size() - 1) {//esto si todavia no es la ultima vuelta se imprime la parte final del rango
                cout << vect2[i] << ", " << endl;
            }
            else if(i == vect2.size()-1){//esta parte es para imprimir si es la ultima pagina y no es consiguiente de las anteriores
                cout << vect2[i] << endl;
            }
        }
    }
    for (int i = 0; i < agregados.size(); i++) {//el tamano de numero de procesos en el sistema, y se usara para ver el tiempo de turnaround al borrarlo
        if (politica == 1) {//si es FIFO
            if (agregados[i].idp == id) {//se checa el id para saber cual es el que se borro y luego se actualiza su turnaround time
                agregados[i].turnaroundFIFO = tiempo - agregados[i].timestamp;
                //las siguientes lineas son para poner el proceso borrado en un lugar para saber su informacion
                liberados.push_back(agregados[i]);
                agregados[i] = agregados[agregados.size() - 1];
                agregados.pop_back();
            }
        }
        else{//si es LRU
            if (agregados2[i].idp == id) {//se checa el id para saber cual es el que se borro y luego se actualiza su turnaround time
                agregados2[i].turnaroundLRU = tiempo2 - agregados2[i].timestamp;
                //las siguientes lineas son para poner el proceso borrado en un lugar para saber su informacion
                liberados2.push_back(agregados2[i]);
                agregados2[i] = agregados2[agregados2.size() - 1];
                agregados2.pop_back();
            }
        }
    }
}

void estadisticas() {
    float sumFIFO = 0;//se usara para sacar los promedio y estadisticas de FIFO
    float sumLRU = 0;//se usara para sacar los promedio y estadisticas de LRU
    float tam1 = liberados.size(), tam2 = liberados2.size();
    while(!liberados.empty()){
        cout << "Tunaround del proceso " << liberados.back().idp << " con politica FIFO: " << liberados.back().turnaroundFIFO << endl;
        sumFIFO = sumFIFO + liberados.back().turnaroundFIFO;
        cout << "Numero de pagefaults del proceso " << liberados.back().idp << " con politica FIFO: " << liberados.back().pagefaultsFIFO << endl;
        liberados.pop_back();
    }

    while(!liberados2.empty()) {
        cout << "Tunaround del proceso " << liberados2.back().idp << " con politica LRU: " << liberados2.back().turnaroundLRU << endl;
        sumLRU = sumLRU + liberados2.back().turnaroundLRU;
        cout << "Numero de pagefaults del proceso " << liberados2.back().idp << " con politica LRU: " << liberados2.back().pagefaultsLRU << endl;
        liberados2.pop_back();
    }

    float promedioFIFO = sumFIFO / tam1;
    float promedioLRU = sumLRU / tam2;

    cout << "Turnaround promedio con politica FIFO: " << promedioFIFO << endl;
    cout << "Turnaround promedio con politica LRU: " << promedioLRU << endl;
    cout << "Numero total de operaciones swap-out y swap-in en FIFO: " << swapsFIFO << endl;
    cout << "Numero total de operaciones swap-out y swap-in en LRU: " << swapsLRU << endl;
}

int main() {
    float M[128][4]; //Arreglo que simula la memoria real para politica FIFO
    float ML[128][4]; //Arreglo que simula la memoria real para politica LRU
    float S[256][4]; //Arreglo auxiliar simulara el area de una memoria real para swapping en FIFO
    float SL[256][4]; //Arreglo auxiliar simulara el area de una memoria real para swapping en LRU

    //Funcion que inicializa la memoria en 0, pues aun no ha llegado ningun proceso
    for (int i = 0; i < 128; i++) {
        M[i][0] = -1;//id
        M[i][1] = -1;//num pagina
        M[i][2] = -1;//timestamp
        M[i][3] = -1;//direccion de memoria
        ML[i][0] = -1;//id
        ML[i][1] = -1;//num pagina
        ML[i][2] = -1;//ultimo uso
        ML[i][3] = -1;//direccion en memoria
    }
    //Funcion que incializa el area de disco reservada para swapping en 0
    for (int i = 0; i < 256; i++) {
        S[i][0] = -1;//id
        S[i][1] = -1;//num pagina
        S[i][2] = -1;//timestamp
        S[i][3] = -1;//direccion en memoria
        SL[i][0] = -1;//id
        SL[i][1] = -1;//num pagina
        SL[i][2] = -1;//ultimo uso
        SL[i][3] = -1;//direccion en memoria
    }

    bool finish = false;//para saber si ya se acabo el programa
    bool error = false; //para saber si se tiene un error en los comandos introducidos por el archivo y avisarle al usuario, sera true mientras haya error
    char process;//se usa para saber que procedimiento usar
    string comentario, archEntradaNombre;//se usara para la lectura del archivo
    cout << "Nombre del archivo (sin extencion (.txt)): ";
    cin >> archEntradaNombre;//se ingresa el nombre del archivo txt
    archEntradaNombre += ".txt";
    ifstream archEnt(archEntradaNombre);
    if (archEnt.is_open()) {//se ve si existe el archivo
        while (!archEnt.eof()) {//si no se ha acabado de leer todo el archivo sigue leyendo
            float bytes = -1, id = -1, mod = -1, direccion = -1, modificacion = -1; //Variables que mandan los usuarios
            archEnt >> process;
            switch (process)
            {
            case 'P':
                error = false;
                archEnt >> bytes;
                archEnt >> id;
                cout << process << " " << bytes << " " << id << endl;
                if(bytes > -1 && id > -1){
                    if (bytes < 2049) {
                        //Output del proceso generado
                        cout << "FIFO: Asignar " << bytes << " bytes al proceso " << id << endl;
                        cargar(bytes, id, M, S, 1);
                        //se imprime lo que se hizo a la consola en FIFO
                        cout << "Se asignaron los marcos de pagina ";
                        for (int k = 0; k < 127; k++) {
                            if (M[k][0] == id) {
                                cout << k;
                                while (M[k][0] == M[k + 1][0]) {
                                    k++;
                                }
                                cout << "-" << k << ", " << endl;
                            }
                        }
                        //Output del proceso cargado
                        cout << "LRU: Asignar " << bytes << " bytes al proceso " << id << endl;
                        cargar(bytes, id, ML, SL, 2);
                        //se imprime lo que se hizo a la consola en LRU
                        cout << "Se asignaron los marcos de pagina ";
                        for (int h = 0; h < 127; h++) {
                            if (ML[h][0] == id) {
                                cout << h;
                                while (ML[h][0] == ML[h + 1][0]) {
                                    h++;
                                    if(h == 127){
                                        break;}
                                }
                                cout << "-" << h << ", " << endl;
                            }
                        }
                    }
                    if(bytes > 2048){ //Si se excede la capacidad de la memoria principal, no se ingresa el proceso
                            cout << "Error: El proceso excede la capacidad de memoria de 2048 bytes\nNo sera procesado" << endl;
                    }
                }
                else{
                    cout << "El comando P esta incompleto o es incorrecto, no sera procesado" << endl;
                }
                break;
            case 'A':
                error = false;
                archEnt >> direccion;
                archEnt >> id;
                archEnt >> modificacion;
                cout << process << " " << direccion << " " << id << " " << modificacion << endl;
                if(direccion > -1 && id > -1 && modificacion > -1){
                    FIFO(direccion, id, modificacion, M, S);
                    LRU(direccion, id, modificacion, ML, SL);
                }
                else{
                    cout << "El comando A esta incompleto o es incorrecto, no sera procesado" << endl;
                }
                break;
            case 'L':
                error = false;
                archEnt >> id;
                cout << process << " " << id << endl;
                if(id > -1){
                    borrarProceso(id, M, S, 1);
                    borrarProceso(id, ML, SL, 2);
                }
                else{
                    cout << "El comando L esta incompleto o es incorrecto, no sera procesado" << endl;
                }
                break;
            case 'C':
                error = false;
                cout << process << endl;
                archEnt >> comentario;
                cout << comentario << endl;
                break;
            case 'F':
                error = false;
                cout << process << endl;
                estadisticas();
                break;
            case 'E':
                error = false;
                cout << process;
                break;
            default:
                if(!error){
                cout << "Error, el comando "  << process << " no es valido\nCualquier entrada siguiente de " << process << " sera ignorada hasta encontrar un input valido" << endl;
                error = true;
                }
                break;
            }
        }
        archEnt.close();
    }
    else {//si no existe el archivo que se ingreso se regresa que no se pudo abrir
        cout << "Archivo no abrio" << endl;
    }
    return EXIT_SUCCESS;
}