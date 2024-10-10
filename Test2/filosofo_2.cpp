// -----------------------------------------------------------------------------
//   Isabel Morro Tabares, 79095945A
//   Grupo A1, Sistemas Concurrentes y Distribuidos 2023/2024
//   Archivo: filosofo_2.cpp
//   
//   FILÓSOFOS CON MPI_Ssend (sin interbloqueo)
//
//    compilar: mpicxx -std=c++11 -o filosofo_2 filosofo_2.cpp
//    ejecutar: mpirun -oversubscribe -np 10 ./filosofo_2
// -----------------------------------------------------------------------------

/*

    EXPLICACIÓN

    Dependiendo de la posición par/impar del filósofo en la mesa, se tomará primero el tenedor derecho
    o el izquierdo. Suponemos que el filosofo con id 0 es 0 es n=0, el filosofo con id 2 es n=1, el filosofo
    con id 4 es n=2, el filósofo con id 6 es n=3 y el filósofo con id 8 es n=4.
    Entonces dependendo si n es par o impar, sabemos que si el filósofo n es par, tomará primero el tenedor
    de la derecha, mientras si el filósofo n es impar, tomará el de la izquierda.
    De esta forma, si todos intentan tomar su primer tenedor, nunca llegarán a tomarlo todos a la misma vez, pues 
    como por ejmeplo en el caso de Platón y Sócrates, coincidirám en un mismo tenedor primero.

    Si es par o impar lo vemo scon el operandor AND.

    Dejo un ejemplo:

        Pitágoras: 0 & 1= F ------ primero toma der 4
        Sócrates:  1 & 1 = T ------- primero toma izq 1
        Platón:    2 & 1 = F --------- primero toma der 1
        Aristóteles: 3 & 1 = T ---- primero tom izq 3
        Epicurio: 4 & 1 = F ------- primero toma der 3

    Podemos ver que coinciden lugeo no existirá interbloqueo.

*/

#include <mpi.h>
#include <thread> // this_thread::sleep_for
#include <random> // dispositivos, generadores y distribuciones aleatorias
#include <chrono> // duraciones (duration), unidades de tiempo
#include <iostream>

using namespace std;
using namespace std::this_thread ;
using namespace std::chrono ;

const int
   num_filosofos = 5 ,              // número de filósofos 
   num_filo_ten  = 2*num_filosofos, // número de filósofos y tenedores 
   num_procesos  = num_filo_ten ;   // número de procesos total (por ahora solo hay filo y ten)


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

void funcion_filosofos( int id_propio )
{
    const int
    id_tenedor_izq = (id_propio+1)              % num_filo_ten,
    id_tenedor_der = (id_propio+num_filo_ten-1) % num_filo_ten;

    int valor  ;

    const int
        id_tenedor_1 = (id_propio/2) & 1 ? id_tenedor_izq : id_tenedor_der , // primer tenedor
        id_tenedor_2 = (id_propio/2) & 1 ? id_tenedor_der : id_tenedor_izq ; // segundo tenedor

    // Obtenemos los nombres del filósofos y sus respectivos tenedores para
    // la salida de mensajes por pantalla
    string  nombre = obtenerNombre (id_propio),
            n_ten_1 = obtenerNomTenedor(id_tenedor_1),
            n_ten_2 = obtenerNomTenedor(id_tenedor_2);


    while( true )
    {

        // 1. HAMBRE

        cout << "Filósofo " << nombre << " tiene hambre y va a empezar a comer" << endl;

        cout << "Filósofo " << nombre << " solicita 1er tenedor " << n_ten_1 << endl;
        // solicitar tenedor 1
        MPI_Ssend( &valor, 1,MPI_INT, id_tenedor_1, 0, MPI_COMM_WORLD );

        cout <<"Filósofo " << nombre << " solicita 2do tenedor " << n_ten_2 << endl;
        // solicitar tenedor 2
        MPI_Ssend( &valor, 1,MPI_INT, id_tenedor_2, 0, MPI_COMM_WORLD );

        // 2. COMER: bloqueo de duración aleatoria

        // espera bloqueada mientras realiza la acción de comer
        cout <<"Filósofo " << nombre << " está comiendo. " << endl;
        sleep_for( milliseconds( aleatorio<10,200>()) );

        // 3. SOLTAR TENEDORES

        // soltar el tenedor 1
        MPI_Ssend( &valor, 1,MPI_INT, id_tenedor_1, 0, MPI_COMM_WORLD );
        cout <<"Filósofo " << nombre << " ha soltado 1er tenedor " << n_ten_1 << endl;

        // soltar el tenedor 2 
        MPI_Ssend( &valor, 1,MPI_INT, id_tenedor_2, 0, MPI_COMM_WORLD );
        cout <<"Filósofo " << nombre << " ha soltado 2do tenedor " << n_ten_2 << endl;

        // 4. PENSAR: bloqueo de duración aleatoria

        // espera bloqueada mientras realiza la acción de pensar
        cout <<"Filósofo " << nombre << " está pensando si tiene hambre" << endl;
        sleep_for( milliseconds( aleatorio<10,200>()) );

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
        // recibir petición de CUALQUIER filósofo
        MPI_Recv ( &valor, 1, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &estado );

        // guardar en 'id_filosofo' el id. del emisor
        id_filosofo = estado.MPI_SOURCE;

        // Obtenemmos le nombre del filósofo que ha cogido el tenedor
        nombre_filosofo = obtenerNombre(id_filosofo);
       
        cout <<"Ten. " << n_ten <<" ha sido cogido por filo. " << nombre_filosofo << endl;

        // recibir liberación de FILÓSOFO CONCRETO, filósofo del cual se recibió el mensaje
        // primero: 'id_filosofo'
        MPI_Recv( &valor, 1, MPI_INT, id_filosofo, 0, MPI_COMM_WORLD, &estado );
        cout <<"Ten. "<< n_ten << " ha sido liberado por filo. " << nombre_filosofo <<endl ;
    }
}

// ---------------------------------------------------------------------

int main ( int argc, char** argv )
{
    int id_propio, num_procesos_actual ;

    srand(time(0));
    MPI_Init( &argc, &argv );
    MPI_Comm_rank( MPI_COMM_WORLD, &id_propio );
    MPI_Comm_size( MPI_COMM_WORLD, &num_procesos_actual );


    if ( num_procesos == num_procesos_actual )
    {
        // ejecutar la función correspondiente a 'id_propio'
        if ( id_propio % 2 == 0 )          // si es par
            funcion_filosofos( id_propio ); //   es un filósofo
        else                               // si es impar
            funcion_tenedores( id_propio ); //   es un tenedor
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
