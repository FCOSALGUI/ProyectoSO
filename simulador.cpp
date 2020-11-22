#include <bits/stdc++.h>

using namespace std;
/*
Simulador de memoria virtual 
Sistemas Operativos
Miembros:
Francisco Salgado Guizar A01365047
Gabriel Gomez Novau A01720009
*/

int M[128]; //Arreglo que simula la memoria real
int S[256]; //Arreglo auxiliar simulara el area de una memoria real para swapping

void cargar(float &bytes, float &id){
    float pagina = ceil(bytes/16); //Variable que calcula la cantidad de marcos de pagina necesarios para el proceso, siempre redondeando para mayor espacio
    for(int i = 0; i < 128;i++){
        if(M[i] == 0 && 127-pagina > 0){
            for(int j = i; j<pagina;j++){
                M[j] = 1;
                cout << j;
            }
            break;
        }

    }
    //cout << "Pagina asignada al proceso: " << id << " es: " << pagina;
}


//Funcion que se encarga de leer el input del usuario y determinar el proceso que quiere llevar a cabo
/*void lectura(char process, int bytes, int id){
    switch (process)
    {
    case 'P':
        break;
    case 'A':
        break;
    case 'L':
        break;
    case 'C':
        break;
    case 'E':
        break;
    default:
        break;
    }
}*/


int main(){
    char process;
    float bytes = 0, id = 0;

    cin >> process;
    cin >> bytes;
    cin >> id;

    switch (process)
    {
    case 'P':
        cargar(bytes,id);
        break;
    case 'A':
        break;
    case 'L':
        break;
    case 'C':
        break;
    case 'F':
        break;
    case 'E':
        break;
    default:
        break;
    } 

    
    return EXIT_SUCCESS;
}