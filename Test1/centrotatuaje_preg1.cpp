/*
    Isabel Morro Tabares, 79095945A
    Grupo A1, Sistemas Concurrentes y Distribuidos 2023/2024

    CENTRO TATUAJES CON SEMÁFOROS

    g++ -std=c++11 -pthread -o centrotatuaje_preg1 centrotatuaje_preg1.cpp scd.cpp

*/

#include <iostream>
#include <cassert>
#include <thread>
#include <mutex>
#include <random>
#include "scd.h"

using namespace std ;
using namespace scd ;

//**********************************************************************
// Variables globales

const int   num_hebras = 10,                                // número de clientes en total

            n_cliente_pequenio = 2,                         // número de hebras de tipo cliente tatuaje pequeño
            n_cliente_mediano = 5,                          // número de hebras de tipo cliente tatuaje mediano
            n_cliente_grande =3,                            // número de hebras de tipo cliente tatuaje grande

            n_tatuador_pequenio = 1,                        // número de tatuador de tatuaje pequeño
            n_tatuador_mediano = 3,                         // número de tatuador de tatuaje mediano
            n_tatuador_grande = 1;                          // número de tatuador de tatuaje grande

int         tatuadores_en_uso = 0;                          // número de tatuadores en uso en total

// Semáforos que indican si el cliente puede comenzar o no a tatuarse con su respectivo tatuador
// Al comienzo todos los tatuadores están libres

Semaphore   semaforo_tatuaje_pequenio = n_tatuador_pequenio,      
            semaforo_tatuaje_mediano = n_tatuador_mediano,       
            semaforo_tatuaje_grande = n_tatuador_grande,

// Semáforo para controlar la salida por pantalla

            v_compartida = 1;

mutex pantalla; // Cerrojo para la salida por pantalla

// Enum class para saber qde que tipo es cada cliente

enum class TIPO {
    pequenio,
    mediano,
    grande
};

// Función para pasar el enum class a string

const string to_string (TIPO tipo){

    switch (tipo)
    {
    case TIPO::pequenio: 
        return "pequeño";
        break;
    case TIPO::mediano:
        return "mediano";
        break;
    case TIPO::grande:
        return "grande";
        break;
    default:
        return "<none>";
        break;
    }

}


//**********************************************************************

// Función tatuarse común a las funciones hebra de todos los clientes

void tatuarse (int num_cliente, TIPO tipo){

    string t = to_string(tipo);

    pantalla.lock();
    cout << "Cliente número " << num_cliente << " con tatuaje " << 
            t << " comienza a tatuarse" << endl;
    pantalla.unlock();

    // Tiempo aleatorio de la duración de la acción tatuarse
    this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    pantalla.lock();
    cout << "Cliente número " << num_cliente << " con tatuaje " << 
            t << " termina de tatuarse" << endl;
    pantalla.unlock();

}

//**********************************************************************

// Función de la hebra de tipo cliente de tatuaje pequeño

void funcion_hebra_tatuaje_pequenio (int num_cliente){

    while (true){

        // Decrementa en uno su semáforo en caso de que este sea mayor que cero
        // Si no es así se queda bloqueada hasta esperar un signal
        sem_wait(semaforo_tatuaje_pequenio);

        // Imprimimos por pantalla
        sem_wait(v_compartida);
            tatuadores_en_uso++;

            pantalla.lock();
            cout << "El número de tatuadores ocupados son: " << tatuadores_en_uso << endl;
            pantalla.unlock();

        sem_signal(v_compartida);

        // La hebra comienza la acción de tatuarse
        tatuarse(num_cliente, TIPO::pequenio);
        
        // Imprimimo por pantalla
        sem_wait(v_compartida);
            tatuadores_en_uso--;

            pantalla.lock();
            cout << "El número de tatuadores ocupados son: " << tatuadores_en_uso << endl;
            pantalla.unlock();

        sem_signal(v_compartida);

        // Damos paso al siguiente que quiera tatuarse (incrementamos el semaforo en 1)
        sem_signal(semaforo_tatuaje_pequenio);

        // Retraso aleatorio fuera del centro de tatuajes
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));

    }
}

//**********************************************************************

// Función de la hebra de tipo cliente de tatuaje mediano

void funcion_hebra_tatuaje_mediano(int num_cliente){

    while (true){

        // Decrementa en uno su semáforo en caso de que este sea mayor que cero
        // Si no es así se queda bloqueada hasta esperar un signal
        sem_wait(semaforo_tatuaje_mediano);

        // Imprimimos por pantalla
        sem_wait(v_compartida);
            tatuadores_en_uso++;

            pantalla.lock();
            cout << "El número de tatuadores ocupados son: " << tatuadores_en_uso << endl;
            pantalla.unlock();

        sem_signal(v_compartida);

        // La hebra comienza la acción de tatuarse
        tatuarse(num_cliente, TIPO::mediano);
        
        // Imprimimos por pantalla
        sem_wait(v_compartida);
            tatuadores_en_uso--;

            pantalla.lock();
            cout << "El número de tatuadores ocupados son: " << tatuadores_en_uso << endl;
            pantalla.unlock();

        sem_signal(v_compartida);

        // Damos paso al siguiente que quiera tatuarse (incrementamos el semaforo en 1)
        sem_signal(semaforo_tatuaje_mediano);

        // Retraso aleatorio fuera del centro de tatuajes
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
    }

}

//**********************************************************************

// Función de la hebra de tipo cliente de tatuaje grande

void funcion_hebra_tatuaje_grande(int num_cliente){

    while (true) {

        // Decrementa en uno su semáforo en caso de que este sea mayor que cero
        // Si no es así se queda bloqueada hasta esperar un signal
        sem_wait(semaforo_tatuaje_grande);

        // Imprimimos por pantalla
        sem_wait(v_compartida);
            tatuadores_en_uso++;

            pantalla.lock();
            cout << "El número de tatuadores ocupados son: " << tatuadores_en_uso << endl;
            pantalla.unlock();

        sem_signal(v_compartida);

        // La hebra comienza la acción de tatuarse
        tatuarse(num_cliente, TIPO::grande);
        
        // Imprimimos por pantalla
        sem_wait(v_compartida);
            tatuadores_en_uso--;

            pantalla.lock();
            cout << "El número de tatuadores ocupados son: " << tatuadores_en_uso << endl;
            pantalla.unlock();

        sem_signal(v_compartida);

        // Damos paso al siguiente que quiera tatuarse (incrementamos el semaforo en 1)
        sem_signal(semaforo_tatuaje_grande);

        // Retraso aleatorio fuera del centro de tatuajes
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
    }
}

//**********************************************************************

int main (){

    // Declaración e inicialización de hebras

    thread  cliente_tatuaje_pequenio[n_cliente_pequenio],
            cliente_tatuaje_mediano[n_cliente_mediano],
            cliente_tatuaje_grande[n_cliente_grande];

    for (int i=0; i < n_cliente_pequenio; i++) cliente_tatuaje_pequenio[i] = thread(funcion_hebra_tatuaje_pequenio, i);
    for (int i=0; i < n_cliente_mediano; i++) cliente_tatuaje_mediano[i] = thread(funcion_hebra_tatuaje_mediano, i);
    for (int i=0; i < n_cliente_grande; i++) cliente_tatuaje_grande[i] = thread(funcion_hebra_tatuaje_grande, i);

    // Finalización de las hebras
    // en realidad en este programa no finalizan porque todas ejecutan un bucle infinito

    for (int i=0; i < n_cliente_pequenio; i++) cliente_tatuaje_pequenio[i].join();
    for (int i=0; i < n_cliente_mediano; i++) cliente_tatuaje_mediano[i].join();
    for (int i=0; i < n_cliente_grande; i++) cliente_tatuaje_grande[i].join();

    return 0;
}