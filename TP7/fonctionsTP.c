void convert_temperature_9(int x,int y) {

    // Si le nombre est positif, afficher « + » sinon afficher « - »
	// Récupérer la valeur absolue en multipliant par -1 un nombre négatif ou en
	// complémentant bit à bit et en ajoutant 1
	if (x < 0) {
		x = x * -1;
		d_write('-');
	} else {
		d_write('+');
	}

	// Traiter la partie entière x
	// Compter le chiffre des dizaines par soustraction par 10
	int i = 0;
	while (x >= 10) {
		x = x - 10;
		i++;
	}
	// A la fin récupérer le chiffre des unités
	// Transformer en caractères ASCII avant de les afficher
	d_write(i + 48);
	d_write(x + 48);

	// Traiter la partie décimale y : les seules valeurs à afficher sont 0 ou 5
	// Afficher caractère par caractère en n’oubliant pas le point entre la partie entière et
	// la partie décimale
	d_write('.');

	if (y >= 128) {
		d_write('5');
	} else {
		d_write('0');
	}

	d_write(' ');
	d_write('C');

}

void convert_temperature_12(int x,int y) {

	if (x < 0) {
		x = x * -1;
		d_write('-');
	} else {
		d_write('+');
	}

	int i = 0;
	while (x >= 10) {
		x = x - 10;
		i++;
	}

	d_write(i + 48);
	d_write(x + 48);

	d_write('.');

	// Traiter la partie décimale y : bien identifier les valeurs à afficher (voir tableau 1)
	switch (y) {
        case 0x00:
			d_write('0');
			d_write('0');
			d_write('0');
			d_write('0');
            break;
        case 0x10:
			d_write('0');
			d_write('6');
			d_write('2');
			d_write('5');
            break;
        case 0x20:
			d_write('1');
			d_write('2');
			d_write('5');
			d_write('0');
            break;
        case 0x80:
			d_write('5');
			d_write('0');
			d_write('0');
			d_write('0');
            break;
        case 0xF0:
			d_write('9');
			d_write('3');
			d_write('7');
			d_write('5');
            break;
        default:
            // Gérer d'autres valeurs de la partie décimale si nécessaire
            d_write('0');
			d_write('0');
			d_write('0');
			d_write('0');
            break;
    }


	d_write(' ');
	d_write('C');
}

void convert_secondes_minutes (int x,int y) {

    // Traiter les minutes y
    // Récupérer le chiffre des dizaines de minutes (i.e. les quatre bits de poids fort
    // de y)
    int i = y >> 4;
    // Récupérer le chiffre des unités de minutes (i.e. les quatre bits de poids faible
    // de y)
    int j = y & 0x0F;
    // Transformer en caractères ASCII avant de les afficher
    d_write(i + 48);
    d_write(j + 48);

    // Traiter les secondes x
    // Récupérer le chiffre des dizaines de secondes (i.e. les quatre bits de poids
    // fort de x)
    int k = x >> 4;
    // Récupérer le chiffre des unités de minutes (i.e. les quatre bits de poids faible
    // de x)
    int l = x & 0x0F;
    // Transformer en caractères ASCII avant de les afficher
    d_write('m');
    d_write('n');
    d_write(k + 48);
    d_write(l + 48);
    d_write('s');

}
