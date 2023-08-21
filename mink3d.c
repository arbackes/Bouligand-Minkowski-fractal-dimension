#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define TAMANHO 256
#define NUM_TOKENS 100

struct ponto3{
    double p[3];
};

struct listaPontos{
    int tam, MAX;
    struct ponto3* po;
};

void executa(char *filename);
int parse(char *str, char *delimiter, char tokens[][TAMANHO]);
int readFileOBJ(char *nome, struct listaPontos* vert);
struct listaPontos* criaListaPontos();
void liberaListaPontos(struct listaPontos* lp);
void insereListaPontos(struct listaPontos *lp, double v0, double v1, double v2);
void normalizaVertices(struct listaPontos* vert);//, int fator);
struct ponto3 minVertice(struct listaPontos* vert);

double *calculaMinkowski3D(struct listaPontos* vert, double raio, int* NPontos);

int maxVetorColuna(struct listaPontos* vert, int idx_col);
void ConverteCoord2Surface(int ***ma3D, struct listaPontos* vert, int raioc);
int*** IniciaMatriz3D(int n_lin, int n_col, int altura);
void EDTLinhas(int ***ma3D, int n_lin, int n_col, int altura);
void EDTColunas(int ***ma3D, int n_lin, int n_col, int altura);
void EDTAltura(int ***ma3D, int n_lin, int n_col, int altura);
int menor(int v1, int v2);
double* Lista_Raios3D(double raio_max, int* nro_raios);
int BuscaPosicao(int raio, double *vet_raios, int nro_pontos);

int main(int argc, char* argv[]){
    if(argc == 1){
        printf("No parameters passed to the program %s\n",argv[0]);
        printf("You must specify the file!\n");
        printf("%s file.obj\n",argv[0]);
    }else{
        int i;
        printf("Processing files...\n");

        for(i=1; i<argc; i++){
            printf("%s ... ",argv[i]);
            executa(argv[i]);
        printf("done\n");
        }
    }

    return 0;
}

void executa(char *filename){
    struct listaPontos *vert;

    //char filename[100] = "d:\\Ahu_1_01_t0.obj";
	char arqSalvar[200];
	strcpy(arqSalvar,filename);
	int N = strlen(filename);
	arqSalvar[N-1] = 't';
	arqSalvar[N-2] = 'x';
	arqSalvar[N-3] = 't';
	//strncpy(arqSalvar,filename,N-3);
	int N1 = strlen(arqSalvar);
	//strcat(arqSalvar,"txt");

    //==========================================================
    vert = criaListaPontos();

    if(!readFileOBJ(filename, vert))
        return;// invalid file.

    normalizaVertices(vert);//, 10);

    //=============================================
    int NPontos;
	double *varea = calculaMinkowski3D(vert, 20.0, &NPontos);

	int nro_raios;
	double *vraio = Lista_Raios3D(20.0, &nro_raios);

    //=============================================
    //FILE *f = fopen("d:\\teste.txt","w");
	FILE *f = fopen(arqSalvar,"w");
    int y;
    for(y = 0; y < NPontos; y++){
        fprintf(f,"%d %.12f %.12f %.12f\n",y,sqrt(vraio[y+1]),log(sqrt(vraio[y+1])),varea[y]);
    }
    fclose(f);
    //=============================================

    liberaListaPontos(vert);
    free(varea);
	free(vraio);
}

int parse(char *str, char *delimiter, char tokens[][TAMANHO]){
    char *tk;
    int cont = 0;
    tk = strtok(str, delimiter);
    while(tk != NULL){
        //printf( " %s\n", token );
        strcpy(tokens[cont],tk);
        cont++;
        tk = strtok(NULL, delimiter);
    }
    return cont;
}

int readFileOBJ(char *nome, struct listaPontos* vert){//, struct listaPontos* faces){
    FILE *f = fopen(nome,"r");
    if(f == NULL){
        printf("Couldn't open file %s\n",nome);
        return 0;
    }
    char str[TAMANHO];
    char tokens[NUM_TOKENS][TAMANHO];

    int qtd;
    while(1){
        if(fgets(str, TAMANHO, f) == NULL)
            break;
        if(feof(f))
            break;

        qtd = parse(str," ", tokens);
        if(strcmp(tokens[0],"v") == 0)
            insereListaPontos(vert, atof(tokens[1]), atof(tokens[2]), atof(tokens[3]));
//        else
//            if(strcmp(tokens[0],"f") == 0)
//                insereListaPontos(faces, atof(tokens[1]), atof(tokens[2]), atof(tokens[3]));

    }
    fclose(f);
    return 1;
}

struct listaPontos* criaListaPontos(){
    struct listaPontos* lp;
    lp = (struct listaPontos*) malloc(sizeof(struct listaPontos));
    lp->MAX = 10;
    lp->tam = 0;
    lp->po = (struct ponto3*) malloc(lp->MAX * sizeof(struct ponto3));
    return lp;
}

void insereListaPontos(struct listaPontos *lp, double v0, double v1, double v2){
    lp->po[lp->tam].p[0] = v0;
    lp->po[lp->tam].p[1] = v1;
    lp->po[lp->tam].p[2] = v2;
    lp->tam++;

    if(lp->tam == lp->MAX){
        lp->MAX = (int) (1.5 * lp->MAX);
		//fprintf(stderr,"Aloc: %d\n",lp->MAX);
        lp->po = (struct ponto3*) realloc(lp->po,lp->MAX * sizeof(struct ponto3));
    }
}

void liberaListaPontos(struct listaPontos* lp){
    free(lp->po);
    free(lp);
}

/*
void normalizaVertices(struct listaPontos* vert, int fator){
    struct ponto3 v;
    int y, x;

    v = minVertice(vert);
    for(y = 0; y < vert->tam; y++){
        for(x = 0; x < 3; x++){
            vert->po[y].p[x] -= v.p[x];
            //vert->po[y].p[x] = round(vert->po[y].p[x] * fator);
			vert->po[y].p[x] = floor(vert->po[y].p[x] * fator + 0.5);
        }
    }
}
*/

void normalizaVertices(struct listaPontos* vert){//, int fator){
    struct ponto3 v;
    int y, x;
    double maior = 0;

    v = minVertice(vert);
    for(y = 0; y < vert->tam; y++){
        for(x = 0; x < 3; x++){
            vert->po[y].p[x] -= v.p[x];
            if(vert->po[y].p[x] > maior)
                maior = vert->po[y].p[x];
        }
    }

    double fator = 255.0/maior;//fator era 10
    for(y = 0; y < vert->tam; y++){
        for(x = 0; x < 3; x++){
            vert->po[y].p[x] = floor(vert->po[y].p[x] * fator + 0.5);
        }
    }
}

struct ponto3 minVertice(struct listaPontos* vert){
  int y, x;
  struct ponto3 v;
  v.p[0] = vert->po[0].p[0];
  v.p[1] = vert->po[0].p[1];
  v.p[2] = vert->po[0].p[2];

  for(y = 0; y < vert->tam; y++){
      for(x = 0; x < 3; x++){
          if(vert->po[y].p[x] < v.p[x]){
              v.p[x] = vert->po[y].p[x];
          }
      }
  }

  return v;
}
//===============================================================

//int maxVetorColuna(double **im, int nro_linhas, int idx_col){
int maxVetorColuna(struct listaPontos* vert, int idx_col){
  int y;
  double v = 0;
  for(y = 0; y < vert->tam; y++){
      if(vert->po[y].p[idx_col] > v){
		  v = vert->po[y].p[idx_col];
	  }
  }

  return (int) v;
}

int*** IniciaMatriz3D(int n_lin, int n_col, int altura){
	int py,px,ph;
	int ***ma3D;

	ma3D = (int ***) malloc (n_lin * sizeof(int **));
	for(py = 0; py < n_lin; py++){
		ma3D[py] = (int **) malloc (n_col * sizeof(int *));
		for(px = 0; px < n_col; px++){
			ma3D[py][px] = (int *) malloc (altura * sizeof(int));
			for(ph = 0; ph < altura; ph++){
				ma3D[py][px][ph] = 1;
			}
		}
	}

	return ma3D;
}

//void ConverteCoord2Surface(int ***ma3D, double **im, int raioc, int nro_lin){
void ConverteCoord2Surface(int ***ma3D, struct listaPontos* vert, int raioc){
  int y, py, px, pz;

  for(y = 0; y < vert->tam; y++){
	  px = (int) vert->po[y].p[0] + raioc;
	  py = (int) vert->po[y].p[1] + raioc;
	  pz = (int) vert->po[y].p[2] + raioc;
	  ma3D[py][px][pz] = 0;
  }

}


int menor(int v1, int v2){
	if (v1 > v2){
		return v2;
	}
	else{
		return v1;
	}
}

void EDTLinhas(int ***ma3D, int n_lin, int n_col, int altura){
//Realiza a EDT nas linhas da matriz...
//Melhor solução: mapear fundo como 1 e imagem como 0!!!
//Desse modo não é necessária uma matriz temporária...
	int ph, plin, pcol, df, db;

	for(ph= 0; ph < altura; ph++){
		//FORWARD SCAN
		//percorre a altura...
		for (plin = 0; plin < n_lin; plin++){
			df = n_col - 1;
			for (pcol = 0; pcol < n_col; pcol++){
				if (ma3D[plin][pcol][ph] != 0){
					df = df + 1;
				}
				else{
					df = 0;
				}

				ma3D[plin][pcol][ph] = df * df;
			}
		}

		//BACKWARD SCAN
		for (plin = 0; plin < n_lin; plin++){
			db = n_col - 1;
			for (pcol = (n_col - 1); pcol >= 0; pcol--){
				if (ma3D[plin][pcol][ph] != 0){
					db = db + 1;
				}
				else{
					db = 0;
				}

				ma3D[plin][pcol][ph] = menor(ma3D[plin][pcol][ph], db*db);
			}
		}
	}
}

void EDTColunas(int ***ma3D, int n_lin, int n_col, int altura){
	int ph, pcol, plin, a, b, nn;
	int mm;
	int *buff;

	//AQUI É n_lin OU n_col ???
	//buff = (int *) malloc (n_col * sizeof(int)); //OLD VERSION -> ERRO!!
	buff = (int *) malloc (n_lin * sizeof(int));

	for (ph = 0; ph < altura; ph++){
		for (pcol = 0; pcol <  n_col; pcol++){//anda nas colunas...
			//preenche buff
			for (plin = 0; plin < n_lin; plin++){
				buff[plin] = ma3D[plin][pcol][ph];
			}
			//FORWARD...
			a = 0;
			for (plin = 1; plin < n_lin; plin++){
				if (a > 0){
					a = a - 1;
				}

				if (buff[plin] > buff[plin - 1] + 1){
					b = (int) ((buff[plin] - buff[plin - 1] - 1) / 2);

					if ((plin + b) > (n_lin - 1)){
					b = (n_lin - 1) - plin;
					}

					for (nn = a; nn <= b; nn++){
						mm = buff[plin - 1] + ((nn + 1) * (nn + 1));
						if (buff[plin + nn] <= mm){
							goto L1;
						}

						if (mm < ma3D[plin + nn][pcol][ph]){
							ma3D[plin + nn][pcol][ph] = mm;
						}
					}

					L1:
					a = b;
				}
				else{
					a = 0;
				}
			}

			//BACKWARD
			a = 0;
			for (plin = n_lin - 2; plin >= 0; plin--){
				if (a > 0){
					a = a - 1;
				}

				if (buff[plin] > buff[plin + 1]){
					b = (int) ((buff[plin] - buff[plin + 1] - 1) / 2);

					if ((plin - b) < 1){
						b = plin;
					}

					for (nn = a; nn <= b; nn++){
						mm = buff[plin + 1] + ((nn + 1) * (nn + 1));
						if (buff[plin - nn] <= mm){
							goto L2;
						}

						if (mm < ma3D[plin - nn][pcol][ph]){
							ma3D[plin - nn][pcol][ph] = mm;
						}
					}

					L2:
					a = b;
				}
				else{
					a = 0;
				}
			}
		}
	}
	free(buff);
}

void EDTAltura(int ***ma3D, int n_lin, int n_col, int altura){
	int ph, pcol, plin, a, b, nn;
	int mm;
	int *buff;

	buff = (int *) malloc (altura * sizeof(int));

	for (plin = 0; plin < n_lin; plin++){
		for (pcol = 0; pcol < n_col; pcol++){//anda nas colunas...
			//preenche buff
			for (ph = 0; ph < altura; ph++){
				buff[ph] = ma3D[plin][pcol][ph];
			}

			//FORWARD...
			a = 0;
			for (ph = 1; ph < altura; ph++){
				if (a > 0){
					a = a - 1;
				}

				if (buff[ph] > buff[ph - 1] + 1){
					b = (int) ((buff[ph] - buff[ph - 1] - 1) / 2);
					if ((ph + b) > (altura - 1)){
						b = (altura - 1) - ph;
					}

					for (nn = a; nn <= b; nn++){
						mm = buff[ph - 1] + ((nn + 1) * (nn + 1));
						if (buff[ph + nn] <= mm){
							goto L1;
						}

						if (mm < ma3D[plin][pcol][ph + nn]){
							ma3D[plin][pcol][ph + nn] = mm;
						}
					}

					L1:
					a = b;
				}
				else{
					a = 0;
				}
			}

			//BACKWARD
			a = 0;
			for (ph = (altura - 2); ph >= 0; ph--){
				if (a > 0){
					a = a - 1;
				}

				if (buff[ph] > buff[ph + 1]){
					b = (int) ((buff[ph] - buff[ph + 1] - 1) / 2);

					if ((ph - b) < 1){
						b = ph;
					}

					for (nn = a; nn <= b; nn++){
						mm = buff[ph + 1] + ((nn + 1) * (nn + 1));
						if (buff[ph - nn] <= mm){
							goto L2;
						}

						if (mm < ma3D[plin][pcol][ph - nn]){
							ma3D[plin][pcol][ph - nn] = mm;
						}
					}

					L2:
					a = b;
				}
				else{
					a = 0;
				}
			}
		}
	}

	free(buff);
}


double* Lista_Raios3D(double raio_max, int* nro_raios){
	 struct cel {
		 int raio;
		 struct cel *prox;
	 };
	 typedef struct cel celula;

	 int i, j, x, y, z, raio, MaximoRaio2;
	 double *vet_raios;

	 celula *lista, *proxima;

	 lista = NULL;

	 MaximoRaio2 = (int) (raio_max * raio_max);
	 i = 0;
	 x = 0;
	 y = 0;
	 z = 0;
	 j = 0;

	 while (j <= MaximoRaio2){
		 if ((x * x) > j){
			 j = j + 1;// não existe esse valor de raio...
			 x = 0;
			 y = 0;
			 z = 0;
		 }
		 else{
			 raio = (int) ((x * x) + (y * y) + (z * z));
			 if (raio == j){
				 i = i + 1;

				 proxima = (celula *) malloc (sizeof(celula));
				 proxima->raio = raio;
				 proxima->prox = lista;
				 lista = proxima;
				 j = j + 1;
				 x = 0;
				 y = 0;
			 }
			 else
			 {
				 z = z + 1;
				 if (z > y){
					 y = y + 1;
					 z = 0;
					 if (y > x){
						 x = x + 1;
						 y = 0;
					 }
				 }
			 }
		 }
	 }

	 //cria o vetor de raios...
	 vet_raios = (double*) malloc (i * sizeof(double));

	 for(j = (i-1); j >= 0; j--){
		 vet_raios[j] = lista->raio;
		 proxima = lista;
		 lista = lista->prox;
		 free(proxima);
	 }

	 *nro_raios = i;
	 return vet_raios;
}

int BuscaPosicao(int raio, double *vet_raios, int nro_pontos){
	int e, d, m;
	//realiza uma busca binária...
	e = 0;
	d = nro_pontos - 1;
	while (e <= d){
		m = (int) ((e + d)/2);
		if (vet_raios[m] == raio){
			return m;
		}

		if (vet_raios[m] < raio){
			e = m + 1;
		}
		else{
			d = m - 1;
		}
	}
}


double *calculaMinkowski3D(struct listaPontos* vert, double raio, int* NPontos){
   int ***ma3D;
   int L, C, H, raioc, nro_raios;
   int i, j, k, m, raiomax2, maxx, maxy, maxz;
   double *vet_raio, *vet_area, *varea, soma;

   raioc = (int) ceil(raio);
   raiomax2 = (int) raioc * raioc;

   maxx = maxVetorColuna(vert,0) + 1;
   maxy = maxVetorColuna(vert,1) + 1;
   maxz = maxVetorColuna(vert,2) + 1;

   L = (int) maxy + 2 * raioc;
   C = (int) maxx + 2 * raioc;
   H = (int) maxz + 2 * raioc;

   ma3D = IniciaMatriz3D(L, C, H);

   //ConverteCoord2Surface(ma3D, im, raioc, nro_lin);
   ConverteCoord2Surface(ma3D, vert, raioc);

   EDTLinhas(ma3D, L, C, H);
   EDTColunas(ma3D, L, C, H);
   EDTAltura(ma3D, L, C, H);

   vet_raio = Lista_Raios3D(raio, &nro_raios);
   vet_area = (double*) malloc (nro_raios * sizeof(double));
   for (i = 0; i < nro_raios; i++){
	   vet_area[i] = 0;
   }

   for (i = 0; i < L; i++){
	   for (j = 0; j < C; j++){
		   for (k = 0; k < H; k++){
			   if (ma3D[i][j][k] <= raiomax2){
				   m = BuscaPosicao((int)ma3D[i][j][k], vet_raio, nro_raios);//m = Busca_Posicao(ma3D[i][j][k]);
				   vet_area[m] = vet_area[m] + 1;
			   }
		   }
	   }
   }

   varea = (double *) malloc((nro_raios - 1)*sizeof(double));
   *NPontos = (nro_raios - 1);

   soma = vet_area[0];
   for (i = 0; i < (nro_raios - 1); i++){
	   soma = soma + vet_area[i + 1];
	   varea[i] = log(soma);
   }

   //limpar variaveis...
   for (i = 0; i < L; i++){
	   for (j = 0; j < C; j++){
			   free(ma3D[i][j]);
	   }
	   free(ma3D[i]);
   }
   free(ma3D);

   free(vet_area);
   free(vet_raio);

   return varea;
}
