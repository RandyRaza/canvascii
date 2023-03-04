/**
 * canvascii.c
 *
 * Ce programme permet de dessiner différentes figures
 * sur un canvas aux dimensions max de 40x80.
 *
 * @author  Randy Razafindrabe
 * @date    10 Février 2022
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#define MAX_HEIGHT 40
#define MAX_WIDTH 80

#define USAGE "\
Usage: ./canvascii [-n HEIGHT,WIDTH] [-s] [-k] [-p CHAR]\n\
          [-h ROW] [-v COL] [-r ROW,COL,HEIGHT,WIDTH]\n\
          [-l ROW1,COL1,ROW2,COL2] [-c ROW,COL,RADIUS]\n\
Draws on an ASCII canvas. The canvas is provided on stdin and\n\
the result is printed on stdout. The dimensions of the canvas\n\
are limited to at most 40 rows and at most 80 columns.\n\
\n\
If no argument is provided, the program prints this help and exit.\n\
\n\
Canvas options:\n\
  -n HEIGHT,WIDTH           Creates a new empty canvas of HEIGHT rows and\n\
                            WIDTH columns. Should be used as first option,\n\
                            otherwise, the behavior is undefined.\n\
                            Ignores stdin.\n\
  -s                        Shows the canvas and exit.\n\
  -k                        Enables colored output. Replaces characters\n\
                            between 0 and 9 by their corresponding ANSI\n\
                            colors:\n\
                              0: black  1: red      2: green  3: yellow\n\
                              4: blue   5: magenta  6: cyan   7: white\n\
\n\
Drawing options:\n\
  -p CHAR                   Sets the pen to CHAR. Allowed pens are\n\
                            0, 1, 2, 3, 4, 5, 6 or 7. Default pen\n\
                            is 7.\n\
  -h ROW                    Draws an horizontal line on row ROW.\n\
  -v COL                    Draws a vertical line on column COL.\n\
  -r ROW,COL,HEIGHT,WIDTH   Draws a rectangle of dimension HEIGHTxWIDTH\n\
                            with top left corner at (ROW,COL).\n\
  -l ROW1,COL1,ROW2,COL2    Draws a discrete segment from (ROW1,COL1) to\n\
                            (ROW2,COL2) with Bresenham's algorithm.\n\
  -c ROW,COL,RADIUS         Draws a circle centered at (ROW,COL) of\n\
                            radius RADIUS with the midpoint algorithm.\n\
"

struct canvas
{
	char pixels[MAX_HEIGHT][MAX_WIDTH]; // A matrix of pixels
	unsigned int width;					// Its width
	unsigned int height;				// Its height
	char pen;							// The character we are drawing with
};

enum error
{
	OK = 0,							// Everything is ok
	ERR_WRONG_PIXEL = 1,			// Wrong pixel value in canvas
	ERR_CANVAS_TOO_HIGH = 2,		// Canvas is too high
	ERR_CANVAS_TOO_WIDE = 3,		// Canvas is too wide
	ERR_CANVAS_NON_RECTANGULAR = 4, // Canvas is non rectangular
	ERR_UNRECOGNIZED_OPTION = 5,	// Unrecognized option
	ERR_MISSING_VALUE = 6,			// Option with missing value
	ERR_WITH_VALUE = 7,				// Problem with value
};

/**
 * Initialise le canvas selon les dimensions passées en ligne de commande.
 * @param matrice, structure canvas à initialiser
 * @param argv, tableau d'arguments de la ligne de commande
 * @param i, index de l'argument de la ligne de commande correspondant à la hauteur et
 * à la largeur du canvas.
 * @return La structure du canvas initialisée.
 */
struct canvas init_canvas(struct canvas matrice, char *argv[], int i)
{
	char *str = strndup(argv[i + 1], strlen(argv[i + 1]));
	char *comma = strchr(str, ',');
	if (comma == NULL)
	{
		fprintf(stderr, "Error: incorrect value with option -n\n");
		fprintf(stderr, "%s", USAGE);
		exit(ERR_WITH_VALUE);
	}
	*comma = '\0';
	matrice.height = atoi(str);
	matrice.width = atoi(comma + 1);
	free(str);

	if (matrice.height > MAX_HEIGHT || matrice.width > MAX_WIDTH)
	{
		fprintf(stderr, "Error: incorrect value with option -n\n");
		printf("%s", USAGE);
		exit(ERR_WITH_VALUE);
	}

	matrice.pen = '7';

	for (int k = 0; k < (int)matrice.height; k++)
	{
		for (int j = 0; j < (int)matrice.width; j++)
		{
			matrice.pixels[k][j] = '.';
		}
	}
	return matrice;
}

/**
 * Affiche le canvas.
 * @param matrice, Structure du canvas à afficher.
 */
void afficher_canvas(struct canvas matrice)
{
	for (int i = 0; i < (int)matrice.height; i++)
	{
		for (int j = 0; j < (int)matrice.width; j++)
		{
			printf("%c", matrice.pixels[i][j]);
		}
		printf("\n");
	}
}

/**
 * Affiche le canvas en appliquant les couleurs correspondant au pen sélectionné.
 * @param matrice, Structure du canvas à afficher.
 */
void afficher_canvas_en_couleur(struct canvas matrice)
{
	for (int i = 0; i < (int)matrice.height; i++)
	{
		for (int j = 0; j < (int)matrice.width; j++)
		{
			char c = (matrice.pixels[i][j] == '.') ? '0' : matrice.pixels[i][j];
			if (c == '0')
				printf("\033[40m \033[0m");
			else if (c == '1')
				printf("\033[41m \033[0m");
			else if (c == '2')
				printf("\033[42m \033[0m");
			else if (c == '3')
				printf("\033[43m \033[0m");
			else if (c == '4')
				printf("\033[44m \033[0m");
			else if (c == '5')
				printf("\033[45m \033[0m");
			else if (c == '6')
				printf("\033[46m \033[0m");
			else if (c == '7')
				printf("\033[47m \033[0m");
			else
				printf("%c", matrice.pixels[i][j]);
		}
		printf("\n");
	}
}

/**
 * Permet de tracer une ligne horizontale sur le canvas.
 * @param matrice, structure canvas à initialiser.
 * @param argv, tableau d'arguments de la ligne de commande.
 * @param rangee, index de l'argument de la ligne de commande correspondant à la rangee sur
 * laquelle dessiner la ligne.
 * @return Le canvas avec la ligne horizontale.
 */
struct canvas tracer_ligne_horizon(struct canvas matrice, char *argv[], int rangee)
{
	if ((atoi(argv[rangee + 1])) < 0 || ((int)atoi(argv[rangee + 1]) >= (int)matrice.height))
	{
		fprintf(stderr, "Error: incorrect value with option -h\n");
		printf("%s", USAGE);
		exit(ERR_WITH_VALUE);
	}
	for (int j = 0; j < (int)matrice.width; j++)
	{
		matrice.pixels[atoi(argv[rangee + 1])][j] = matrice.pen;
	}
	return matrice;
}

/**
 * Permet de tracer une ligne verticale sur le canvas.
 * @param matrice, structure canvas à initialiser.
 * @param argv, tableau d'arguments de la ligne de commande.
 * @param colonne, index de l'argument de la ligne de commande correspondant à la colonne.
 * sur laquelle dessiner la ligne.
 * @return Le canvas avec la ligne verticale.
 */
struct canvas tracer_ligne_verticale(struct canvas matrice, char *argv[], int colonne)
{

	if ((atoi(argv[colonne + 1])) < 0 || ((int)atoi(argv[colonne + 1])) >= (int)matrice.width)
	{
		fprintf(stderr, "Error: incorrect value with option -v\n");
		printf("%s", USAGE);
		exit(ERR_WITH_VALUE);
	}
	for (int i = 0; i < (int)matrice.width; i++)
	{
		matrice.pixels[i][atoi(argv[colonne + 1])] = matrice.pen;
	}
	return matrice;
}

/**
 * Permet de tracer un rectangle sur le canvas.
 * @param matrice, structure canvas à initialiser.
 * @param argv, tableau d'arguments de la ligne de commande.
 * @param ligne, index de l'argument de la ligne de commande correspondant à la position
 * x0.
 * @return Le canvas avec le rectangle dessiné.
 */
struct canvas tracer_rectangle(struct canvas matrice, char *argv[], int ligne)
{
	int debut_ligne = atoi(strtok(argv[ligne + 1], ","));
	int debut_colonne = atoi(strtok(NULL, ","));
	int height = atoi(strtok(NULL, ","));
	int width = atoi(strtok(NULL, " "));

	int max_ligne = debut_ligne + height;
	int max_colonne = debut_colonne + width;

	if (height < 0 || width < 0)
	{
		fprintf(stderr, "Error: incorrect value with option -r\n");
		printf("%s", USAGE);
		exit(ERR_WITH_VALUE);
	}

		for (int i = debut_ligne; i < max_ligne; i++)
		{
			for (int j = debut_colonne; j < max_colonne; j++)
			{
				char temp = matrice.pixels[i][j];
				if (i == debut_ligne || i == max_ligne - 1 || j == debut_colonne || j == max_colonne - 1)
					matrice.pixels[i][j] = matrice.pen;
				else
					matrice.pixels[i][j] = temp;
			}
		}
		 
	
	return matrice;
}

/**
 * Permet de tracer un segment sur le canvas.
 * @param matrice, structure canvas à initialiser.
 * @param argv, tableau d'arguments de la ligne de commande.
 * @param ligne, index de l'argument de la ligne de commande correspondant à la position
 * x0.
 * @link l'algorithme de Bresenham qui permet de tracer le segment https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
 * @return Le canvas avec le segment dessiné.
 */
struct canvas tracer_segment(struct canvas matrice, char *argv[], int ligne)
{
	int debut_ligne = atoi(strtok(argv[ligne + 1], ","));
	int debut_colonne = atoi(strtok(NULL, ","));
	int fin_ligne = atoi(strtok(NULL, ","));
	int fin_colonne = atoi(strtok(NULL, " "));
	int dx = abs(fin_ligne - debut_ligne);
	int sx = debut_ligne < fin_ligne ? 1 : -1;
	int dy = -abs(fin_colonne - debut_colonne);
	int sy = debut_colonne < fin_colonne ? 1 : -1;
	int error = dx + dy;

	while (1)
	{
		matrice.pixels[debut_ligne][debut_colonne] = matrice.pen;
		if (debut_ligne == fin_ligne && debut_colonne == fin_colonne)
			break;
		int e2 = 2 * error;
		if (e2 >= dy)
		{
			if (debut_ligne == fin_ligne)
				break;
			error = error + dy;
			debut_ligne = debut_ligne + sx;
		}
		if (e2 <= dx)
		{
			if (debut_colonne == fin_colonne)
				break;
			error = error + dx;
			debut_colonne = debut_colonne + sy;
		}
	}
	return matrice;
}

/**
 * Permet de tracer un cercle sur le canvas.
 * @param matrice, structure canvas à initialiser.
 * @param argv, tableau d'arguments de la ligne de commande.
 * @param ligne, index de l'argument de la ligne de commande correspondant à la position
 * x0.
 * @link L'algorithme de tracé d'arc de cercle par point milieu https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
 * @return Le canvas avec le cercle dessiné.
 */
struct canvas tracer_cercle(struct canvas matrice, char *argv[], int ligne)
{
	int debut_ligne = atoi(strtok(argv[ligne + 1], ","));
	int debut_colonne = atoi(strtok(NULL, ","));
	int rayon = atoi(strtok(NULL, " "));
	int f = 1 - rayon;
	int ddF_x = 0;
	int ddF_y = -2 * rayon;
	int x = 0;

	matrice.pixels[debut_ligne][debut_colonne + rayon] = matrice.pen;
	matrice.pixels[debut_ligne][debut_colonne - rayon] = matrice.pen;
	matrice.pixels[debut_ligne + rayon][debut_colonne] = matrice.pen;
	matrice.pixels[debut_ligne - rayon][debut_colonne] = matrice.pen;

	if (rayon < 0)
	{
		fprintf(stderr, "Error: incorrect value with option -c\n");
		printf("%s", USAGE);
		exit(7);
	}
	while (x < rayon)
	{
		if (f >= 0)
		{
			rayon--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;
		matrice.pixels[debut_ligne + x][debut_colonne + rayon] = matrice.pen;
		matrice.pixels[debut_ligne - x][debut_colonne + rayon] = matrice.pen;
		matrice.pixels[debut_ligne + x][debut_colonne - rayon] = matrice.pen;
		matrice.pixels[debut_ligne - x][debut_colonne - rayon] = matrice.pen;
		matrice.pixels[debut_ligne + rayon][debut_colonne + x] = matrice.pen;
		matrice.pixels[debut_ligne - rayon][debut_colonne + x] = matrice.pen;
		matrice.pixels[debut_ligne + rayon][debut_colonne - x] = matrice.pen;
		matrice.pixels[debut_ligne - rayon][debut_colonne - x] = matrice.pen;
	}
	return matrice;
}

// /**
//  * Fonction qui gère toute les options que le programme permet d'utiliser
//  * @param argc, représente le nombre d'arguments dans la ligne de commande utilisée pour exécuter le programme.
//  * @param argv, ableau d'arguments de la ligne de commande.
//  */
void gestion_options(int argc, char *argv[])
{
	struct canvas matrice;
	matrice.width = 0;
	matrice.height = 0;
	matrice.pen = '7';

	if (!isatty(STDIN_FILENO))
	{
		int largeur = 0;
		int hauteur = 0;
		int ligne = 0;
		char character = 0;

		while ((character = fgetc(stdin)) != EOF)
		{
			if (largeur > MAX_WIDTH)
			{
				fprintf(stderr, "Error: canvas is too wide (max width: 80)\n");
				printf("%s", USAGE);
				exit(ERR_CANVAS_TOO_WIDE);
			}

			if (character == '#')
			{
				fprintf(stderr, "Error: wrong pixel value #\n");
				printf("%s", USAGE);
				exit(ERR_WRONG_PIXEL);
			}
			if (character == '\n')
			{
				hauteur++;
				if (ligne > largeur || ligne < largeur)
				{
					fprintf(stderr, "Error: canvas should be rectangular\n");
					printf("%s", USAGE);
					exit(ERR_CANVAS_NON_RECTANGULAR);
				}
				ligne = 0;
				if (hauteur > MAX_HEIGHT)
			{
				fprintf(stderr, "Error: canvas is too high (max height: 40)\n");
				printf("%s", USAGE);
				exit(ERR_CANVAS_TOO_HIGH);
			}
			}
			else
			{
				matrice.pixels[hauteur][ligne] = character;
				ligne++;
				largeur = (ligne > largeur) ? ligne : largeur;
			}
		}
		matrice.height = hauteur;
		matrice.width = largeur;
	}
	if (argc == 1)
	{
		printf("%s", USAGE);
	}
	else
	{
		for (int i = 0; i < argc; i++)
		{
			if (strcmp(argv[i], "-n") == 0)
			{
				if (i + 1 >= argc)
				{
					fprintf(stderr, "Error: missing value with option -n\n");
					fprintf(stderr, "%s", USAGE);
					exit(ERR_MISSING_VALUE);
				}
				else

					matrice = init_canvas(matrice, argv, i);
			}
			else if (strcmp(argv[i], "-s") == 0)
			{
				afficher_canvas(matrice);
				exit(OK);
			}
			else if (strcmp(argv[i], "-k") == 0)
			{
				afficher_canvas_en_couleur(matrice);
				exit(OK);
			}
			else if (strcmp(argv[i], "-p") == 0)
			{
				if (!isdigit(*argv[i + 1]))
				{
					fprintf(stderr, "Error: incorrect value with option -p\n");
					fprintf(stderr, "%s", USAGE);
					exit(ERR_WITH_VALUE);
				}
				else
				{
					if (atoi(argv[i + 1]) < 1 || atoi(argv[i + 1]) > 7)
					{
						fprintf(stderr, "Error: incorrect value after option -p\n");
						fprintf(stderr, "%s", USAGE);
						exit(ERR_WITH_VALUE);
					}
					else
						matrice.pen = argv[i + 1][0];
				}
			}
			else if (strcmp(argv[i], "-h") == 0)
			{
				matrice = tracer_ligne_horizon(matrice, argv, i);
			}
			else if (strcmp(argv[i], "-v") == 0)
			{
				matrice = tracer_ligne_verticale(matrice, argv, i);
			}
			else if (strcmp(argv[i], "-r") == 0)
			{
				matrice = tracer_rectangle(matrice, argv, i);
			}
			else if (strcmp(argv[i], "-l") == 0)
			{
				matrice = tracer_segment(matrice, argv, i);
			}
			else if (strcmp(argv[i], "-c") == 0)
			{
				matrice = tracer_cercle(matrice, argv, i);
			}
			else if (strcmp(argv[i], "-a") == 0)
			{
				fprintf(stderr, "Error: unrecognized option -a\n");
				fprintf(stderr, "%s", USAGE);
				exit(ERR_UNRECOGNIZED_OPTION);
			}
		}
		afficher_canvas(matrice);
	}

}


/**
 * Main
 */
int main(int argc, char *argv[])
{
	gestion_options(argc, argv);
	return (0);
}
