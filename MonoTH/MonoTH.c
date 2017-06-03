/*
 * main.c
 *
 *  Created on: 05/05/2017
 *      Author: ezequiel
 */

/**
   @file main.c
   @brief
   @author
   @date
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

struct Pulso
{
  float pol_fase;
  float pol_cuadratura;
};

void
cantidad_datos(void);
void
almacenar_datos(struct Pulso *pulsos_vertical[], struct Pulso *pulsos_horizontal[]);
void
guardar_pulsos(struct Pulso *pulsos_vertical[], struct Pulso *pulsos_horizontal[]);
void
guardar_gate_pulsos(int cant_datos, struct Pulso *gate_Vertical[][((int) (0.5 * cant_datos) / 250) + 1],
    struct Pulso *gate_Horizontal[][((int) (0.5 * cant_datos) / 250) + 1]);
void
    gate_Pulsos(int cant_datos, struct Pulso *pulsos_M[],
        struct Pulso *gate_Estruc[][((int) (0.5 * cant_datos) / 250) + 1]);
float
get_autocorrelacion(int cant_datos, struct Pulso *gate_Estruc[][((int) (0.5 * cant_datos) / 250) + 1]);

FILE *source;
float dato;
int n = 0, j = 0;
short int cant_datos;
struct Pulso *pulso = NULL;

int
main()
{
  for(int i = 0;i<30;i++) {
  float autocorrelacion_h = 0;
  float autocorrelacion_v = 0;

  cantidad_datos();
  //printf("Cantidad de datos %d\n", cant_datos);
  struct Pulso *pulsos_vertical[2 * cant_datos];
  struct Pulso *pulsos_horizontal[2 * cant_datos];
  almacenar_datos(pulsos_vertical, pulsos_horizontal);
  //guardar_pulsos(pulsos_vertical, pulsos_horizontal);
  int gate = (0.5 * cant_datos) / 250;
  //printf("Cantidad de muestras por gate: %d\n", gate);
  struct Pulso *gate_Vertical[(int) cant_datos / gate][gate + 1];
  struct Pulso *gate_Horizontal[(int) cant_datos / gate][gate + 1];
  gate_Pulsos(cant_datos, pulsos_vertical, gate_Vertical);
  gate_Pulsos(cant_datos, pulsos_horizontal, gate_Horizontal);
  //guardar_gate_pulsos(cant_datos, gate_Vertical, gate_Horizontal);
  autocorrelacion_v = get_autocorrelacion(cant_datos, gate_Vertical);
  autocorrelacion_h = get_autocorrelacion(cant_datos, gate_Horizontal);
  /*
   * El formato del archivo binario es el siguiente:
   *            [string]  -> Valor que diferencia entre vertical y horizontal
   *            [float]   -> Valor numerico
   */
  FILE *binario;
  binario = fopen("autocorrelacion.bin","wb");
  fprintf(binario,"AC_V\n%f\n",autocorrelacion_v);
  fprintf(binario,"AC_H\n%f\n",autocorrelacion_h);
  fclose(binario);
  }
  return 0;
}

/**
   @brief Lee el archivo binario y determina la cantidad de datos (Valid Sample)
   que hay. Almacena la cantidad en 'cant_datos'.

 */
void
cantidad_datos(void)
{
  source = fopen("../pulsos.iq", "rb");
  if (source)
    {
      n = fread(&cant_datos, sizeof(short int), 1, source);
    }
  else
    {
      printf("Fallo\n");
    }
  fclose(source);
  return;

}

/**
   @brief Almacena cada valid sample en su correspondiente estructura. Si tenemos 'n' muestras,
   se tienen '2*n' registros por tipo de pulso según la estructura del archivo 'pulsos.iq'.
   Cada registro (i%2)==0 corresponde a un pulso en fase, en caso contrario es en cuadratura.

   @param *pulsos_vertical[] contiene cada pulso vertical con su respectiva componente en fase
   y cuadratura.
   @param *pulsos_horizontal[] contiene cada pulso horizontal con su respectiva componente en fase
   y cuadratura.

 */
void
almacenar_datos(struct Pulso *pulsos_vertical[], struct Pulso *pulsos_horizontal[])
{
  source = fopen("../pulsos.iq", "rb");
  if (source)
    {
      int i, j=0;
      n = fread(&cant_datos, sizeof(short int), 1, source);
      //---------------VERTICAL---------------;
      fseek(source, 2, SEEK_SET);
      for (i = 0; i < 2 * cant_datos; i++)
        {
          pulso = malloc(sizeof(struct Pulso));
          n = fread(&dato, sizeof(float), 1, source);
          pulso->pol_fase = dato; //PAR
          n = fread(&dato, sizeof(float), 1, source);
          pulso->pol_cuadratura = dato; //IMPAR
          i++;
          pulsos_vertical[j] = pulso;
          j++;
        }
      j = 0;
      //---------------HORIZONTAL---------------;
      for (i = 0; i <= 2 * cant_datos; i++)
        {
          pulso = malloc(sizeof(struct Pulso));
          n = fread(&dato, sizeof(float), 1, source);
          pulso->pol_fase = dato; //PAR
          n = fread(&dato, sizeof(float), 1, source);
          pulso->pol_cuadratura = dato; //IMPAR
          i++;
          pulsos_horizontal[j] = pulso;
          j++;
        }
    }
  else
    {
      printf("Fallo\n");
    }
  fclose(source);
  free(pulso);
  return;
}

/**
   @brief Calcula la cantidad de muestras por gate segun la relación (rango_resolucion*validSample/rango_cobertura),
   donde rango_resolucion=0.5km, rango_cobertura=250km. Cada gate almacena el promedio de las muestras que caen
   dentro de el.

   @param cant_datos Cantidad de validSample.
   @param *pulsos_M[] Estructura que contiene cada pulso con su respectiva componente en fase y cuadratura.
   @param *gate_Estruc[][] Estructura que almacenara cada gate y las muestras que incluye de la forma
   [promedio][cantidad], donde promedio es la media de las 'n' muestras incluidas y cantidad es el numero
   de muestras por gate.
   Para 3 muestras por gate:
                 [Re(media)+jIm(media))][Re(vS1)+jIm(vS1)][Re(vS2)+jIm(vS2)][Re(vS3)+jIm(vS3)]

 */
void
gate_Pulsos(int cant_datos, struct Pulso *pulsos_M[], struct Pulso *gate_Estruc[][((int) (0.5 * cant_datos) / 250) + 1])
{
  int gate = (0.5 * cant_datos) / 250;
  int aux = 0, j = 0, i;
  float real = 0, img = 0;
  memset(*gate_Estruc, '\0', cant_datos);

  for (i = 0; i < cant_datos; i++)
    {
      if (aux < gate)
        {
          real = real + pulsos_M[i]->pol_fase;
          img = img + pulsos_M[i]->pol_cuadratura;
          aux++;
          gate_Estruc[j][aux] = pulsos_M[i];
        }
      else
        {
          pulso = malloc(sizeof(struct Pulso));
          pulso->pol_fase = real / (aux);
          pulso->pol_cuadratura = img / (aux);
          gate_Estruc[j][0] = pulso;
          i--;
          j++;
          aux = 0;
          real = img = 0;
        }
    }
  aux = 0;
  real = img = 0;
  free(pulso);
}

/**
   @brief Cálculo de autocorrelacion sobre los 'n' gates. Realizara el cálculo para la señal
   horizontal o vertical según la estructura que se pase como parámetro.

   @param cant_datos Cantidad de validSample.
   @param *gate_Estruc[][] Estructura que contiene cada gate.

   @returns El valor del calculo de autocorrelación.

 */
float
get_autocorrelacion(int cant_datos, struct Pulso *gate_Estruc[][((int) (0.5 * cant_datos) / 250) + 1])
{
  int gate = (0.5 * cant_datos) / 250;
  int i;
  float v_resultado = 0;

  for (i = 1; i < ((int) cant_datos / gate) - 1; i++)
    {
      v_resultado = v_resultado + (sqrt(pow(gate_Estruc[i][0]->pol_fase, 2)+pow(gate_Estruc[i-1][0]->pol_cuadratura, 2))
      							* (sqrt(pow(gate_Estruc[i][0]->pol_fase, 2)+pow(gate_Estruc[i-1][0]->pol_cuadratura, 2))));
    }
  v_resultado = v_resultado / cant_datos;
  //printf("Autocorrelacion:%f\n", v_resultado);
  return v_resultado;

}

/**
   @brief Genera un archivo .txt con las componentes en fase y cuadratura de cada pulso
   horizontal y vertical.

   @param *pulsos_vertical[] Estructura que contiene cada pulso vertical con su respectiva
   componente en fase y cuadratura.
   @param *pulsos_horizontal[] Estructura que contiene cada pulso horizontal con su respectiva
   componente en fase y cuadratura.

 */
void
guardar_pulsos(struct Pulso *pulsos_vertical[], struct Pulso *pulsos_horizontal[])
{
  FILE *salida;
  salida = fopen("salida.txt", "w");
  fprintf(salida, "%13sVERTICAL %20sHORIZONTAL\n", "", "");
  fprintf(salida, "%11sFase %6sCuadratura | %4sFase %6sCuadratura\n", "", "", "", "");
  for (int j = 0; j < cant_datos; j++)
    {
      fprintf(salida, "%4d)   %10f    %10f | %10f    %10f\n", j, pulsos_vertical[j]->pol_fase,
          pulsos_vertical[j]->pol_cuadratura, pulsos_horizontal[j]->pol_fase, pulsos_horizontal[j]->pol_cuadratura);
    }
  fclose(salida);
  return;
}

/**
   @brief Genera un archivo .txt con información del valor de cada gate y de las
   muestras que contiene.

   @param cant_datos Cantidad de validSample.
   @param *gate_Vertical[][] Estructura que contiene cada gate vertical con sus
   respectivas muestras.
   @param *gate_Horizontal[][] Estructura que contiene cada gate horizontal con sus
   respectivas muestras.

 */
void
guardar_gate_pulsos(int cant_datos, struct Pulso *gate_Vertical[][((int) (0.5 * cant_datos) / 250) + 1],
    struct Pulso *gate_Horizontal[][((int) (0.5 * cant_datos) / 250) + 1])
{
  int gate = (0.5 * cant_datos) / 250;
  FILE *gate_file;
  gate_file = fopen("gate.txt", "w");
  for (int j = 0; j < (cant_datos / gate); j++)
    {
      fprintf(gate_file, "%d) Media %10f %10f %20f %10f\n", j, gate_Vertical[j][0]->pol_fase,
          gate_Vertical[j][0]->pol_cuadratura, gate_Horizontal[j][0]->pol_fase, gate_Horizontal[j][0]->pol_cuadratura);
      for (int k = 0; k < gate; k++)
        {
          fprintf(gate_file, "%d) %10f %10f %20f %10f\n", k, gate_Vertical[j][k + 1]->pol_fase,
              gate_Vertical[j][k + 1]->pol_cuadratura, gate_Horizontal[j][k + 1]->pol_fase,
              gate_Horizontal[j][k + 1]->pol_cuadratura);
        }
    }
  fclose(gate_file);
  return;
}

