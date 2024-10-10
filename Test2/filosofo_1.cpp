// -----------------------------------------------------------------------------
//   Isabel Morro Tabares, 79095945A
//   Grupo A1, Sistemas Concurrentes y Distribuidos 2023/2024
//   Archivo: filosofos_1.cpp
//   
//   FILÓSOFOS CON MPI_Ssend (existe interbloqueo)
//
//    compilar: mpicxx -std=c++11 -o filosofos_1 filosofos_1.cpp
//    ejecutar: mpirun -oversubscribe -np 10 ./filosofos_1
// -----------------------------------------------------------------------------


#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>
#include <string>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
    num_filosofos = 5 ,                 // número de filósofos 
    num_filo_ten  = 2*num_filosofos,    // número de filósofos y tenedores 
    num_procesos  = num_filo_ten;       // número de procesos total (por ahora solo hay filo y ten)


//**********************************************************************
// plantilla de función para generar un entero aleatorio uniformemente
// distribuido entre dos valores enteros, ambos incluidos
// (ambos tienen que ser dos constantes, conocidas en tiempo de compilación)
//----------------------------------------------------------------------

template< int min, int max > int aleatorio()
{
   static default_random_engine generador( (random_device())() );
   static uniform_int_distribution<int> distribucion_uniforme( min, max ) ;
   return distribucion_uniforme( generador );
}

// ---------------------------------------------------------------------

// Función para obtener los nombres de los filósofos según
// su identificador

string obtenerNombre (int id){

    string nombre;

    switch (id){
        case 0:
            nombre = "Pitágoras";
            break;
        case 2:
            nombre = "Sócrates";
            break;
        case 4:
            nombre = "Platón";
            break;
        case 6:
            nombre = "Aristóteles";
            break;
        case 8:
            nombre = "Epicuro";
            break;
    }

    return nombre;
}

// ---------------------------------------------------------------------

// Función para obtener el nombre de los tenedores según 
// su identificador

string obtenerNomTenedor (int id){

    string nombre;

    switch (id){
        case 1:
            nombre = "T0";
            break;
        case 3:
            nombre = "T1";
            break;
        case 5:
            nombre = "T2";
            break;
        case 7:
            nombre = "T3";
            break;
        case 9:
            nombre = "T4";
            break;
    }

    return nombre;

}

// ---------------------------------------------------------------------

void funcion_filosofos( int id )
{
    int     id_ten_izq = (id+1)                 % num_filo_ten, //id. tenedor izq.
            id_ten_der = (id+num_filo_ten-1)    % num_filo_ten; //id. tenedor der.
    int valor = 1;

    // Obtenemos los nombres del filósofos y sus respectivos tenedores para
    // la salida de mensajes por pantalla
    string  nombre = obtenerNombre(id),
            n_ten_der = obtenerNomTenedor(id_ten_der),
            n_ten_izq = obtenerNomTenedor(id_ten_izq);


    while ( true )
    {
        // 1. HAMBRE

        cout << "Filósofo " << nombre << " tiene hambre y va a empezar a comer" << endl;

        cout << "Filósofo " << nombre << " solicita 1er tenedor " << n_ten_izq << " (izq.)" << endl;
        // solicitar tenedor izquierdo
        MPI_Ssend (&valor, 1, MPI_INT, id_ten_izq, 0 , MPI_COMM_WORLD);

        cout <<"Filósofo " << nombre << " solicita 2do tenedor " << n_ten_der << " (der.)" << endl;
        // solicitar tenedor derecho
        MPI_Ssend (&valor, 1, MPI_INT, id_ten_der, 0 , MPI_COMM_WORLD);

        // 2. COMER: bloqueo de duración aleatoria

        // espera bloqueada mientras realiza la acción de comer
        cout <<"Filósofo " << nombre << " está comiendo. " << endl;
        sleep_for( milliseconds( aleatorio<10,100>() ) );

        // 3. SOLTAR TENEDORES

        // soltar el tenedor izquierdo
        MPI_Ssend (&valor, 1, MPI_INT, id_ten_izq, 0 , MPI_COMM_WORLD);
        cout <<"Filósofo " << nombre << " ha soltado 1er tenedor " << n_ten_izq << "(izq.)" << endl;

        // soltar el tenedor derecho 
        MPI_Ssend( &valor, 1, MPI_INT, id_ten_der, 0, MPI_COMM_WORLD );
        cout <<"Filósofo " << nombre << " ha soltado 2do tenedor " << n_ten_der << "(der.)" << endl;

        // 4. PENSAR: bloqueo de duración aleatoria

        // espera bloqueada mientras realiza la acción de pensar
        cout <<"Filósofo " << nombre << " está pensando si tiene hambre" << endl;
        sleep_for( milliseconds( aleatorio<10,100>() ) );
    }
}
// ---------------------------------------------------------------------

void funcion_tenedores( int id )
{
    int valor, id_filosofo ;  // valor recibido, identificador del filósofo
    MPI_Status estado ;       // metadatos de las dos recepciones

    // Obtenemos nombre del tenedor para la salida del mensaje por pantalla
    string nombre_filosofo;
    string n_ten = obtenerNomTenedor(id);

    while ( true )
    {
        // recibir petición de cualquier filósofo
        MPI_Recv ( &valor, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &estado );

        // guardar en 'id_filosofo' el id. del emisor
        id_filosofo = estado.MPI_SOURCE;
        // Obtenemmos le nombre del filósofo que ha cogido el tenedor
        nombre_filosofo = obtenerNombre(id_filosofo);
        cout <<"Ten. " << n_ten <<" ha sido cogido por filo. " << nombre_filosofo << endl;

        // espera a recibir liberación de filósofo 'id_filosofo'
        MPI_Recv( &valor, 1, MPI_INT, id_filosofo, 0, MPI_COMM_WORLD, &estado );
        cout <<"Ten. "<< n_ten << " ha sido liberado por filo. " << nombre_filosofo << endl ;
    }
}

// ---------------------------------------------------------------------

int main( int argc, char** argv )
{
    int id_propio, num_procesos_actual ;

    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
    MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


    if ( num_procesos == num_procesos_actual )
    {
        // ejecutar la función correspondiente a 'id_propio'
        if ( id_propio % 2 == 0 )               // si es par
            funcion_filosofos( id_propio );     // es un filósofo
        else                                    // si es impar
            funcion_tenedores( id_propio );     // es un tenedor
    }
    else
    {
        if ( id_propio == 0 ) // solo el primero escribe error, indep. del rol
        { cout << "el número de procesos esperados es:    " << num_procesos << endl
                << "el número de procesos en ejecución es: " << num_procesos_actual << endl
                << "(programa abortado)" << endl ;
        }
    }

    // Finalizamos zona MPI
    MPI_Finalize( );
    
    return 0;
}

// ---------------------------------------------------------------------
