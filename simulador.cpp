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
    int idp;
    float timestamp;
    proceso(int idt, float timestampt) {
        idp = idt;
        timestamp = timestampt;
    }
    int paginas;
    int pagefaultsLRU = 1;
    int pagefaultsFIFO = 1;
    float turnaroundFIFO = 0;
    float turnaroundLRU = 0;
};

float tiempo = 0;//variable para ir manejando el tiempo que llevan los procesos FIFO
float tiempo2 = 0;//variable para ir manejando el tiempo que llevan los procesos LRU

int swapsFIFO = 0;
int swapsLRU = 0;

vector<proceso> agregados;//vector que guardara los procesos agregados a memoria para luego revisarlos 
vector<proceso> liberados; //vector que guarda a los procesos que se vayan liberando de memoria para luego usar como estadisticas de turnaround y pagefaults

//Funcion que hace el swapping unicamente para procesos recien creados que no quepan en memoria principal
void swap_creados(int pagina, float id, float(&M)[128][4], float(&S)[256][4], int politica, bool creado) {
    int cont = 0;// se usara para ir contando las paginas nuevas
    while (pagina > 0) {//mientras que no se acabe de cargar el proceso en swap a la memoria real no se acaba
        float temporal = 999999;// para comparar el timestamp y ver cuales es el first in en memoria real
        int tempoIndice = -1;//para saber que indice es el que tiene el timestamp mas bajo
        for (int i = 0; i < 128; i++) {
            if (M[i][2] < temporal) {//se checa cual es el first in the todas las paginas
                temporal = M[i][2];
                tempoIndice = i;
            }
        }
        for (int i = 0; i < 256; i++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
            if (S[i][0] == -1) {
                S[i][0] = M[tempoIndice][0];//el id del proceso swapeado
                S[i][1] = M[tempoIndice][1]; //el numero de pagina del proceso
                if (politica == 1) {
                    S[i][2] = tiempo;//el timestamp al momento de swapear
                    tiempo++;//se incrementa el tiempo por swapear a out
                    swapsFIFO++;
                }
                else {
                    S[i][2] = tiempo2;//el timestamp al momento de swapear
                    tiempo2++;//se incrementa el tiempo por swapear a out
                    swapsLRU++;
                }

                S[i][3] = M[tempoIndice][3];//se ve cual es la direccion en memoria
                cout << "Pagina " << S[i][1] << " del proceso " << S[i][0] << " swappeada al marco " << i << " del area de swapping" << endl;
                break;
            }
        }
        M[tempoIndice][0] = id;//se cambia el id al nuevo proceso
        M[tempoIndice][1] = cont;//se asigna el numero de pagina del proceso
        cont++;
        if (politica == 1) {
            M[tempoIndice][2] = tiempo;//time stap de la pagina
            tiempo++;
            if (creado) {
                agregados.back().pagefaultsFIFO++;
            }
        }
        else
        {
            M[tempoIndice][2] = tiempo2;//time stap de la pagina
            tiempo2++;
            if (creado) {
                agregados.back().pagefaultsLRU++;
            }
        }

        if (!creado) {//se checa si es el proceso es nuevo o ya fue agregado al vector de agregados
            proceso temp(id, tiempo);
            temp.paginas = pagina;
            creado = true;
            agregados.push_back(temp);//se agrega el nuevo proceso creado
        }
        M[tempoIndice][3] = tempoIndice;// se asigna el la direccion de memoria
        pagina--;//se va ir a la siguiente pagina a cambiar
    }
}

void cargar(float bytes, float id, float(&M)[128][4], float(&S)[256][4], int politica) {
    bool creado = false;
    int pagina = ceil(bytes / 16); //Variable que calcula la cantidad de marcos de pagina necesarios para el proceso, siempre redondeando para mayor espacio
    int tempPaginas = pagina;
    int cont = 0;
    for (int i = 0; i < 128; i++) { //Loop que recorre toda la memoria y en caso de encontrar el espacio necesario, guarda el proceso en la memoria
        if (M[i][0] == -1) {//Revisa desde donde se encuentra el espacio vacio mas las paginas necesarias que no se pase de la memoria
            M[i][0] = id;
            M[i][1] = cont;
            cont++;
            if (politica == 1) {
                M[i][2] = tiempo;
                tiempo++;
            }
            else {
                M[i][2] = tiempo2;
                tiempo2++;
            }
            M[i][3] = i;
            pagina--;
            if (!creado) {
                proceso Nuevo(id, tiempo); //Creacion de un nuevo proceso, con su respectivo ID proveido por el usuario
                Nuevo.paginas = tempPaginas;
                agregados.push_back(Nuevo);
                creado = true;
            }
        }
        if (i == 127) {//En caso de que no se pueda meter el proceso, se hace swapping
            //El 1 indica politica de reemplazo FIFO y el 2 LRU 
            if (politica == 1) {
                swap_creados(pagina, id, M, S, politica, creado);
            }
            else {
                swap_creados(pagina, id, M, S, politica, creado);
            }
            pagina = 0;
        }
        if (pagina == 0) {
            /*for(int x = 0; x<128;x++){
                //cout << "ID del proceso: " << M[x][0] << endl;
                cout << "Numero del pagina del proceso " << M[x][0] << " es " << M[x][1]<<  endl;
                cout << "Timestamp del proceso " << M[x][0] << " es " << M[x][2]<< endl;
                cout << "Direccion de una pagina del proceso " << M[x][0] << " es " << M[x][3]<< endl;
            }*/

            break;
        }
    }
}

void FIFO(int direccion, int id, int modificacion, float(&M)[128][4], float(&S)[256][4]) {
    int pagina = floor(direccion / 16);
    int real = 0;
    int menor = 999999;
    int ind = 0;
    cout << "Obtener la direccion real correspondiente a la direccion virtual " << direccion << " del proceso " << id << endl;
    for (int k = 0; k < 128; k++) {
        if (M[k][0] == id && M[k][1] == pagina) {
            real = (direccion % 16) + k * 16;
            if (modificacion == 1) {
                cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
            }
            cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
            tiempo += 0.1;
            break;
        }

        if (k == 127) {
            for (int i = 0; i < agregados.size(); i++) {
                if (agregados[i].idp == id) {
                    agregados[i].pagefaultsFIFO++;
                }
            }
            for (int i = 0; i < 256; i++) {
                if (S[i][0] == id && S[i][1] == pagina) {
                    for (int j = 0; j < 128; j++) {
                        if (M[j][2] < menor) {
                            menor = M[j][2];
                            ind = j;
                        }
                        for (int x = 0; x < 256; x++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
                            if (S[x][0] == -1) {
                                S[x][0] = M[ind][0];//el id del proceso swapeado
                                S[x][1] = M[ind][1]; //el numero de pagina del proceso
                                S[x][2] = tiempo;//el timestamp al momento de swapear
                                tiempo++;//se incrementa el tiempo por swapear a out
                                S[x][3] = M[ind][3];//se ve cual es la direccion en memoria

                                M[ind][0] = S[i][0];
                                M[ind][1] = S[i][1];
                                M[ind][2] = tiempo;
                                tiempo++;
                                M[ind][3] = S[i][3];

                                S[i][0] = -1;
                                S[i][1] = -1;
                                S[i][2] = -1;
                                S[i][3] = -1;

                                cout << "Pagina " << S[x][1] << " del proceso " << S[x][0] << " swapeada al marco " << x << " del area de swapping" << endl;
                                cout << "Se localizo la pagina " << M[ind][1] << " que estaba en la posicion " << i << " de swapping y cargo al marco " << ind << endl;
                                real = (direccion % 16) + k * 16;
                                if (modificacion == 1) {
                                    cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
                                }
                                cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
                                tiempo += 0.1;
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
    int pagina = floor(direccion / 16);
    int real = 0;
    int menor = 999999;
    int ind = 0;
    cout << "Obtener la direccion real correspondiente a la direccion virtual " << direccion << " del proceso " << id << endl;
    for (int k = 0; k < 128; k++) {
        if (M[k][0] == id && M[k][1] == pagina) {
            real = (direccion % 16) + k * 16;
            if (modificacion == 1) {
                cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
            }
            cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
            M[k][2] = tiempo2;
            tiempo2 += 0.1;
            break;
        }

        if (k == 127) {
            for (int i = 0; i < agregados.size(); i++) {
                if (agregados[i].idp == id) {
                    agregados[i].pagefaultsLRU++;
                }
            }
            for (int i = 0; i < 256; i++) {
                if (S[i][0] == id && S[i][1] == pagina) {
                    for (int j = 0; j < 128; j++) {
                        if (M[j][2] < menor) {
                            menor = M[j][2];
                            ind = j;
                        }
                        for (int x = 0; x < 256; x++) {//Se busca encontrar un espacio en memoria virtual para poder guardar los procesos swapeados
                            if (S[x][0] == -1) {
                                S[x][0] = M[ind][0];//el id del proceso swapeado
                                S[x][1] = M[ind][1]; //el numero de pagina del proceso
                                S[x][2] = tiempo2;//el timestamp al momento de swapear
                                tiempo2++;//se incrementa el tiempo por swapear a out
                                S[x][3] = M[ind][3];//se ve cual es la direccion en memoria

                                M[ind][0] = S[i][0];//el id del proceso swapeado
                                M[ind][1] = S[i][1]; //el numero de pagina del proceso
                                M[ind][2] = tiempo2;//el timestamp al momento de swapear
                                tiempo2++;//se incrementa el tiempo por swapear a out
                                M[ind][3] = S[i][3];//se ve cual es la direccion en memoria

                                S[i][0] = -1;
                                S[i][1] = -1;
                                S[i][2] = -1;
                                S[i][3] = -1;

                                cout << "Pagina " << S[x][1] << " del proceso " << S[x][0] << " swapeada al marco " << x << " del area de swapping" << endl;
                                cout << "Se localizo la pagina " << M[ind][1] << " que estaba en la posicion " << i << " de swapping y cargo al marco " << ind << endl;
                                real = (direccion % 16) + k * 16;
                                if (modificacion == 1) {
                                    cout << "y modificar dicha direccion" << "\nPagina " << M[k][1] << " del proceso " << M[k][0] << " modificada " << endl;
                                }
                                cout << "Direccion virtual: " << direccion << " Direccion real: " << real << endl;
                                tiempo += 0.1;
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
    cout << "liberar los marcos de paginas ocupados por el proceso: " << id << endl;;
    vector<int> vect;
    for (int i = 0; i < 128; i++) {
        if (M[i][0] == id) {
            M[i][0] = -1;
            M[i][1] = -1;
            M[i][2] = -1;
            M[i][3] = -1;
            vect.push_back(i);
            if (politica == 1) {
                tiempo += 0.1;
            }
            else {
                tiempo2 += 0.1;
            }
        }
    }
    if (vect.size() == 1) {
        cout << "Se liberan los marcos de pagina de memoria real " << vect[0] << endl;;
    }
    else if (vect.size() > 1) {
        cout << "Se liberan los marcos de pagina de memoria real ";
        for (int i = 0; i < vect.size() - 1; i++) {
            cout << vect[i];
            while (vect[i] == vect[i + 1] - 1) {
                i++;
            }
            if (i != vect.size() - 1) {
                cout << "-" << vect[i - 1] << ", ";
            }
            else {
                cout << vect[i] << ", ";
            }
        }
    }
    for (int i = 0; i < 256; i++) {
        if (S[i][0] == id) {
            S[i][0] = -1;
            S[i][1] = -1;
            S[i][2] = -1;
            S[i][3] = -1;
            vect.push_back(i);
            if (politica == 1) {
                tiempo += 0.1;
            }
            else {
                tiempo2 += 0.1;
            }
        }
    }
    vector<int> vect2;
    if (vect2.size() == 1) {
        cout << "Se liberan los marcos de pagina del area de swapping " << vect2[0] << endl;;
    }
    else if (vect2.size() > 1) {
        cout << "Se liberan los marcos de pagina del area de swapping ";
        for (int i = 0; i < vect2.size() - 1; i++) {
            cout << vect2[i];
            while (vect2[i] == vect2[i + 1] - 1) {
                i++;
            }
            if (i != vect2.size() - 1) {
                cout << "-" << vect2[i - 1] << ", ";
            }
            else {
                cout << vect2[i] << ", ";
            }
        }
    }
    for (int i = 0; i < agregados.size(); i++) {
        if (agregados[i].idp == id) {
            if (politica == 1) {
                agregados[i].turnaroundFIFO = tiempo - agregados[i].timestamp;
            }
            else
            {
                agregados[i].turnaroundLRU = tiempo2 - agregados[i].timestamp;
            }

            liberados.push_back(agregados[i]);
            agregados[i] = agregados[agregados.size() - 1];
            agregados.pop_back();
        }
    }

}

void estadisticas() {
    float sumFIFO = 0;
    float sumLRU = 0;
    for (int i = 0; i < liberados.size(); i++) {
        cout << "Tunaround del proceso " << liberados[i].idp << " con politica FIFO: " << liberados[i].turnaroundFIFO << endl;
        sumFIFO += liberados[i].turnaroundFIFO;
        cout << "Tunaround del proceso " << liberados[i].idp << " con politica LRU: " << liberados[i].turnaroundLRU << endl;
        sumLRU += liberados[i].turnaroundLRU;
        cout << "Numero de pagefaults del proceso " << liberados[i].idp << " con politica FIFO: " << liberados[i].pagefaultsFIFO << endl;
        cout << "Numero de pagefaults del proceso " << liberados[i].idp << " con politica LRU: " << liberados[i].pagefaultsLRU << endl;
    }
    float promedioFIFO = sumFIFO / (liberados.size());
    float promedioLRU = sumLRU / (liberados.size());
    //cout << "sumas: " << sumFIFO << sumLRU <<endl;
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

    /*queue<proceso> FIFO; // Queue que guarda los procesos para la politica FIFO
    queue<proceso> LRU; // Queue que guarda los procesos para la politica LRU
    vector<proceso> SFIFO; //Dichos vectores guardaran a los procesos que vayan siendo mandados a swapping
    vector<proceso> SLRU; */

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

    bool finish = false;
    char process;
    float bytes = 0, id = 0, mod = 0, direccion = 0, modificacion = 0;
    string comentario, archEntradaNombre;
    cout << "Nombre del archivo (sin extencion): ";
    cin >> archEntradaNombre;
    archEntradaNombre += ".txt";
    ifstream archEnt(archEntradaNombre);
    if (archEnt.is_open()) {
        while (!archEnt.eof()) {
            archEnt >> process;
            switch (process)
            {
            case 'P':
                archEnt >> bytes;
                archEnt >> id;
                //Output del proceso generado
                cout << process << " " << bytes << " " << id << endl;
                cout << "FIFO: Asignar " << bytes << " bytes al proceso " << id << endl;
                cargar(bytes, id, M, S, 1);
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
                cout << "Se asignaron los marcos de pagina ";
                for (int h = 0; h < 127; h++) {
                    if (ML[h][0] == id) {
                        cout << h;
                        while (ML[h][0] == ML[h + 1][0]) {
                            h++;
                        }
                        cout << "-" << h << ", " << endl;
                    }
                }
                break;
            case 'A':
                archEnt >> direccion;
                archEnt >> id;
                archEnt >> modificacion;
                cout << process << " " << direccion << " " << id << " " << modificacion << endl;
                FIFO(direccion, id, modificacion, M, S);
                LRU(direccion, id, modificacion, ML, SL);
                break;
            case 'L':
                archEnt >> id;
                cout << process << " " << id << endl;
                borrarProceso(id, M, S, 1);
                borrarProceso(id, ML, SL, 2);
                break;
            case 'C':
                cout << process << endl;
                archEnt >> comentario;
                cout << comentario << endl;
                break;
            case 'F':
                cout << process << endl;
                estadisticas();
                break;
            case 'E':
                cout << process << endl;
                break;
            default:
                cout << "Error, el comando pues no es valido" << endl;
                break;
            }
        }
        archEnt.close();
    }
    else {
        cout << "archivo no abrio" << endl;
    }
    return EXIT_SUCCESS;
}