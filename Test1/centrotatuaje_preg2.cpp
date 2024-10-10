/*
    Isabel Morro Tabares, 79095945A
    Grupo A1, Sistemas Concurrentes y Distribuidos 2023/2024

    CENTRO TATUAJES CON MONITOR

    g++ -std=c++11 -pthread -o centrotatuaje_preg2 centrotatuaje_preg2.cpp scd.cpp

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

//**********************************************************************

// Monitor del Centro de Tatuajes

class CentroDeTatuajes : public HoareMonitor
{
    private:

        // Variables de condición para saber cuando puede entrar o no un cliente a tatuarse
        CondVar tatuador_pequenio,
                tatuador_mediano,
                tatuador_grande;

        // Variable que contabiliza el número de tatuadores en uso de cada tipo
        int tatuadores_en_uso_pequenio,
            tatuadores_en_uso_mediano,
            tatuadores_en_uso_grande;

    public:

        // Constructor

        CentroDeTatuajes();

        // Métodos de entrada y salida del centro de cada tipo de hebra

        void entra_cliente_pequenio (int num_cliente);
        void entra_cliente_mediano (int num_cliente);
        void entra_cliente_grande (int num_cliente);

        void sale_cliente_pequenio (int num_cliente);
        void sale_cliente_mediano (int num_cliente);
        void sale_cliente_grande (int num_cliente);

};

// ------------------------------------------------------------------------------------

CentroDeTatuajes::CentroDeTatuajes(){
    
    tatuador_pequenio = newCondVar();
    tatuador_mediano = newCondVar();
    tatuador_grande = newCondVar();

    tatuadores_en_uso_pequenio = 0;
    tatuadores_en_uso_mediano = 0;
    tatuadores_en_uso_grande = 0;
}

// ------------------------------------------------------------------------------------

void CentroDeTatuajes::entra_cliente_pequenio (int num_cliente){

    cout << "Cliente con número " << num_cliente << " de tipo tatuaje pequeño " <<
            " entra a tatuarse" << endl;

    // Si no hay tatuadores libres de su tipo espera hasta poder pasar a tatuarse
    if (tatuadores_en_uso_pequenio == n_tatuador_pequenio)
        tatuador_pequenio.wait();
    
    // Incrementa el número de tatuadores usados de su tipo
    tatuadores_en_uso_pequenio++;

    cout << "Cliente número " << num_cliente << " con tatuaje pequeño " << 
        " comienza a tatuarse" << endl;

    cout << "                   Tatuadores de tipo pequeño ocupados: " << 
        tatuadores_en_uso_pequenio << endl;

}

// ------------------------------------------------------------------------------------

void CentroDeTatuajes::entra_cliente_mediano (int num_cliente){

    cout << "Cliente con número " << num_cliente << " de tipo tatuaje mediano " <<
            " entra a tatuarse" << endl;

    // Si no hay tatuadores libres de su tipo espera hasta poder pasar a tatuarse
    if (tatuadores_en_uso_mediano == n_tatuador_mediano)
        tatuador_mediano.wait();
    
    // Incrementa el número de tatuadores usados de su tipo
    tatuadores_en_uso_mediano++;

    cout << "Cliente número " << num_cliente << " con tatuaje mediano " << 
        " comienza a tatuarse" << endl;

    cout << "                   Tatuadores de tipo mediano ocupados: " << 
        tatuadores_en_uso_mediano << endl;

}

// ------------------------------------------------------------------------------------

void CentroDeTatuajes::entra_cliente_grande (int num_cliente){

    cout << "Cliente con número " << num_cliente << " de tipo tatuaje grande " <<
            " entra a tatuarse" << endl;

    // Si no hay tatuadores libres de su tipo espera hasta poder pasar a tatuarse
    if (tatuadores_en_uso_grande == n_tatuador_grande)
        tatuador_grande.wait();
    
    // Incrementa el número de tatuadores usados de su tipo
    tatuadores_en_uso_grande++;

    cout << "Cliente número " << num_cliente << " con tatuaje grande " << 
        " comienza a tatuarse" << endl;

    cout << "                   Tatuadores de tipo grande ocupados: " << 
        tatuadores_en_uso_grande << endl;

}

// ------------------------------------------------------------------------------------

void CentroDeTatuajes::sale_cliente_pequenio (int num_cliente){

    cout << "Cliente número " << num_cliente << " con tatuaje pequeño " << 
            " termina de tatuarse" << endl;

    // Decrementa el número de tatuadores usados de su tipo
    tatuadores_en_uso_pequenio--;

    cout << "                   Tatuadores de tipo pequeño ocupados: " << 
        tatuadores_en_uso_pequenio << endl;

    cout << "Cliente con número " << num_cliente << " de tipo tatuaje pequeño " <<
            " sale de tatuarse" << endl;

    // Permite quien este esperando a tatuarse se desbloquee
    tatuador_pequenio.signal();
}


// ------------------------------------------------------------------------------------

void CentroDeTatuajes::sale_cliente_mediano (int num_cliente){

    cout << "Cliente número " << num_cliente << " con tatuaje mediano " << 
            " termina de tatuarse" << endl;

    // Decrementa el número de tatuadores usados de su tipo
    tatuadores_en_uso_mediano--;

    cout << "                   Tatuadores de tipo mediano ocupados: " << 
        tatuadores_en_uso_mediano << endl;

    cout << "Cliente con número " << num_cliente << " de tipo tatuaje mediano " <<
            " sale de tatuarse" << endl;

    // Permite quien este esperando a tatuarse se desbloquee
    tatuador_mediano.signal();
}


// ------------------------------------------------------------------------------------

void CentroDeTatuajes::sale_cliente_grande (int num_cliente){

    cout << "Cliente número " << num_cliente << " con tatuaje graande " << 
            " termina de tatuarse" << endl;

    // Decrementa el número de tatuadores usados de su tipo
    tatuadores_en_uso_grande--;

    cout << "                   Tatuadores de tipo grande ocupados: " << 
        tatuadores_en_uso_grande << endl;

    cout << "Cliente con número " << num_cliente << " de tipo tatuaje grande " <<
            " sale de tatuarse" << endl;

    // Permite quien este esperando a tatuarse se desbloquee
    tatuador_grande.signal();
}


//**********************************************************************

// Funciones de las hebras

void funcion_hebra_tatuaje_pequenio(MRef<CentroDeTatuajes> monitor, int num_cliente){

    while (true){

        monitor->entra_cliente_pequenio(num_cliente);

        // Tiempo de espera aleatorio de la acción tatuar
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
        monitor->sale_cliente_pequenio(num_cliente);

        // Tiempo de espera aleatorio fuera del Centro de Tatuajes
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
    }

}

// ------------------------------------------------------------------------------------

void funcion_hebra_tatuaje_mediano(MRef<CentroDeTatuajes> monitor, int num_cliente){

    while (true){

        monitor->entra_cliente_mediano(num_cliente);

        // Tiempo de espera aleatorio de la acción tatuar
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
        monitor->sale_cliente_mediano(num_cliente);

        // Tiempo de espera aleatorio fuera del Centro de Tatuajes
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
    }

}

// ------------------------------------------------------------------------------------

void funcion_hebra_tatuaje_grande(MRef<CentroDeTatuajes> monitor, int num_cliente){

    while (true){

        monitor->entra_cliente_grande(num_cliente);

        // Tiempo de espera aleatorio de la acción tatuar
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
        monitor->sale_cliente_grande(num_cliente);

        // Tiempo de espera aleatorio fuera del Centro de Tatuajes
        this_thread::sleep_for( chrono::milliseconds( aleatorio<20,100>() ));
    }
}

// ------------------------------------------------------------------------------------

int main (){

    // Declaración e inicialización del monitor
    
    MRef <CentroDeTatuajes> monitor = Create<CentroDeTatuajes>();

    // Declaración e inicialización de hebras

    thread  cliente_tatuaje_pequenio[n_cliente_pequenio],
            cliente_tatuaje_mediano[n_cliente_mediano],
            cliente_tatuaje_grande[n_cliente_grande];

    for (int i=0; i < n_cliente_pequenio; i++) 
        cliente_tatuaje_pequenio[i] = thread(funcion_hebra_tatuaje_pequenio,monitor, i);
    for (int i=0; i < n_cliente_mediano; i++) 
        cliente_tatuaje_mediano[i] = thread(funcion_hebra_tatuaje_mediano,monitor, i);
    for (int i=0; i < n_cliente_grande; i++) 
        cliente_tatuaje_grande[i] = thread(funcion_hebra_tatuaje_grande,monitor, i);

    // Finalización de las hebras
    // en realidad en este programa no finalizan porque todas ejecutan un bucle infinito

    for (int i=0; i < n_cliente_pequenio; i++) cliente_tatuaje_pequenio[i].join();
    for (int i=0; i < n_cliente_mediano; i++) cliente_tatuaje_mediano[i].join();
    for (int i=0; i < n_cliente_grande; i++) cliente_tatuaje_grande[i].join();

    return 0;
}
