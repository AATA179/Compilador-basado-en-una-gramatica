#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <GL/glut.h>
#include <regex>
#include <unordered_map>

#define M_PI 3.14159265358979323846 // Definición de PI para dibujar los circulos del arbol

using namespace std;

// Estructura para los nodos del árbol ternario
struct Nodo {
    string valor;
    Nodo* izquierdo;
    Nodo* medio;
    Nodo* derecho;
    Nodo* padre;

    // Constructor del nodo
    Nodo(const string& val) : valor(val), izquierdo(nullptr), medio(nullptr), derecho(nullptr), padre(nullptr) {}
};

// Contenedores globales para separar los tokens y valores numéricos
vector<string> cadena;
vector<string> valores;
vector<Nodo*> ultimos;
vector<string> operaciones;

// Prototipo de la funcion errores
void errores(int);

// Función para verificar si una cadena representa un número
bool esNumero(const string& str) {
    std::regex numero_regex("^[0-9]*\\.?[0-9]+$");
    return regex_match(str, numero_regex);
}

// Clase para manejar un árbol ternario
class ArbolTernario {
public:
    Nodo* raiz; // Nodo raíz del árbol

    // Constructor
    ArbolTernario() : raiz(nullptr) {}

    // Inserta un nuevo nodo en el árbol
    void insertar(const string& val, Nodo*& nodo, Nodo* padre = nullptr) {
        if (nodo == nullptr) {
            nodo = new Nodo(val);
            nodo->padre = padre;
        }
    }

    // Calcula la altura del árbol
    int altura(Nodo* raiz) {
        if (raiz == nullptr) {
            return 0;
        }
        else {
            int alturaIzquierda = altura(raiz->izquierdo);
            int alturaMedia = altura(raiz->medio);
            int alturaDerecha = altura(raiz->derecho);
            return 1 + max({ alturaIzquierda, alturaMedia, alturaDerecha });
        }
    }

    // Dibuja una línea entre dos puntos
    void dibujarLinea(float x1, float y1, float x2, float y2) {
        glColor3f(1.0f, 1.0f, 1.0f); // Color de las líneas (blanco)
        glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glEnd();
    }

    // Dibuja un nodo en la pantalla
    void dibujarNodo(float x, float y, const std::string& valor) {
        if (valor != "+" && valor != "*" && valor != "(" && valor != ")" && valor != "NULL" && valor != "num" && !esNumero(valor)) {
            // Dibujar un círculo para nodos que no son terminales
            float radius = 0.05f;
            glColor3f(0.0f, 0.0f, 1.0f); // Color del círculo azul
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(x, y);
            for (int i = 0; i <= 100; i++) {
                float angle = 2 * M_PI * i / 100;
                glVertex2f(x + cos(angle) * radius, y + sin(angle) * radius);
            }
            glEnd();
        }

        // Dibujar el valor del nodo
        glColor3f(1.0f, 1.0f, 1.0f); // Color del texto (blanco)
        glRasterPos2f(x - 0.02f, y - 0.02f);
        for (char c : valor) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
        }
    }

    // Dibuja el árbol entero
    void dibujarArbol(Nodo* raiz, float x, float y, float offsetX, float offsetY) {
        if (raiz == nullptr) return;

        // Dibujar líneas hacia los hijos
        if (raiz->izquierdo) {
            float hijoX = x - offsetX;
            float hijoY = y - offsetY;
            dibujarLinea(x, y, hijoX, hijoY);
            dibujarArbol(raiz->izquierdo, hijoX, hijoY, offsetX * 0.7f, offsetY);
        }

        if (raiz->medio) {
            float hijoX = x;
            float hijoY = y - offsetY;
            dibujarLinea(x, y, hijoX, hijoY);
            dibujarArbol(raiz->medio, hijoX, hijoY, offsetX * 0.7f, offsetY);
        }

        if (raiz->derecho) {
            float hijoX = x + offsetX;
            float hijoY = y - offsetY;
            dibujarLinea(x, y, hijoX, hijoY);
            dibujarArbol(raiz->derecho, hijoX, hijoY, offsetX * 0.7f, offsetY);
        }

        // Dibujar el nodo actual
        dibujarNodo(x, y, raiz->valor);
    }

};

// Instancia global del árbol ternario y variable de offset
ArbolTernario arbol;
float offsetX = 0.5f; // Valor inicial del offset en el eje X (Espacio entre nodos del mismo nivel)

// Inicializa la configuración de OpenGL
void initOpenGL() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Fondo negro
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // Coordenadas ortográficas
}

// Función de visualización de OpenGL
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float x = 0.0f;
    float y = 0.9f;
    float offsetY = 0.2f; // Espacio entre niveles del arbol
    arbol.dibujarArbol(arbol.raiz, x, y, offsetX, offsetY);
    glFlush();
}

/* Función para aumentar o disminuir el espacio entre nodos sobre X
para evitar que los nodos se encimen o se salgan de la pantalla*/
void teclado(unsigned char key, int x, int y) {
    if (key == '+') {
        offsetX += 0.1f; // Aumentar offsetX
    }
    else if (key == '-') {
        offsetX -= 0.1f; // Disminuir offsetX
    }
    else if (key == 27) {
        exit(0);
    }
    glutPostRedisplay(); // Redibuja la ventana
}

// Tokenizar la cadena entrante, si detecta un simbolo no valido manda a errores
void lexer(const string& entrada) {
    size_t aux = 0;
    string agregar;
    string caracteres_validos = "0123456789.+*()-/";

    for (int i = 0; i < entrada.size(); i++) {
        if (caracteres_validos.find(entrada[i]) == string::npos && entrada[i] != ' ') {
            agregar = entrada[i];
            cadena.push_back(agregar);
            errores(1); // Si encuentra un caracter no valido llama a la funcion errores
        }
    }

    while (aux < entrada.size()) {

        // Encontrar el siguiente operador o paréntesis
        size_t pos = entrada.find_first_of(" +*()-/", aux);
        
        // Si no encuentra más operadores, procesa el resto de la cadena
        if (pos == std::string::npos) {
            agregar = entrada.substr(aux);
            if (!agregar.empty()) {
                cadena.push_back(agregar);
            }
            break;
        }

        // Si hay texto antes del operador, lo agregamos como número o token
        if (pos > aux) {
            agregar = entrada.substr(aux, pos - aux);
            cadena.push_back(agregar);
        }

        // Si el operador no es un espacio, lo agregamos como token
        if (entrada[pos] != ' ') {
            agregar = entrada.substr(pos, 1);
            cadena.push_back(agregar);
        }

        aux = pos + 1;
    }

    // Convertir números al token 'num' y almacenarlos en 'valores'
    for (int i = 0; i < cadena.size(); i++) {
        size_t pos2 = cadena[i].find_first_not_of("0123456789.");
        if (pos2 == string::npos) {  // Es un número
            valores.push_back(cadena[i]);
            cadena[i] = "num";  // Reemplaza el número por el token 'num'
        }
    }
}

// Si la cadena tokenizada es ambigua manda a error y no se procesa
void encontrarAmbiguedad() {
    for (int i = 0; i < cadena.size(); i++) {
        if (cadena[i] == "+" || cadena[i] == "-") {
            if (i + 2 < cadena.size() && (cadena[i + 2] == "+" || cadena[i + 2] == "-")) {
                errores(2);
                break;
            }
            else if (cadena[i + 1] == "(") {
                for (int j = i; j < cadena.size(); j++) {
                    if (cadena[j] == ")" && (j + 1 < cadena.size() && (cadena[j + 1] == "+" || cadena[j + 1] == "-"))) {
                        errores(2);
                        break;
                    }
                }
                break;
            }
        }
        else if (cadena[i] == "*" || cadena[i] == "/") {
            if (i + 2 < cadena.size() && (cadena[i + 2] == "*" || cadena[i + 2] == "/")) {
                errores(2);
                break;
            }
            else if (cadena[i + 1] == "(") {
                for (int j = i; j < cadena.size(); j++) {
                    if (cadena[j] == ")" && (j + 1 < cadena.size() && (cadena[j + 1] == "*" || cadena[j + 1] == "/"))) {
                        errores(2);
                        break;
                    }
                }
                break;
            }
        }
    }
}

// Función para construir el árbol a partir de la cadena tokenizada
/* Se manejaron las reglas de produccion de la gramatica a partir de if else */
void parser() {
    arbol.insertar("<E>", arbol.raiz);
    Nodo* actual = arbol.raiz;

    while (!cadena.empty()) {
        if (actual == nullptr) break;

        if (actual->valor == "<E>") {
            string op = "-1";
            size_t i;
            for (i = 0; i < cadena.size(); i++) {
                if (cadena[i] == "+") {
                    op = "+";
                    break;
                }
                else if (cadena[i] == "-" && i != 0) {
                    if (cadena[i - 1] == "num") {
                        op = "-";
                        break;
                    }
                }
                else if (cadena[i] == "(") {
                    break;
                }
            }

            if (op == "+") {
                arbol.insertar("<E>", actual->izquierdo, actual);
                arbol.insertar("+", actual->medio, actual);
                arbol.insertar("<T>", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                actual = actual->izquierdo;
            }
            else if (op == "-") {
                arbol.insertar("<E>", actual->izquierdo, actual);
                arbol.insertar("-", actual->medio, actual);
                arbol.insertar("<T>", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                actual = actual->izquierdo;
            }
            else {
                arbol.insertar("NULL", actual->izquierdo, actual);
                arbol.insertar("<T>", actual->medio, actual);
                arbol.insertar("NULL", actual->derecho, actual);
                actual = actual->medio;
            }
        }
        else if (actual->valor == "<T>") {
            string op = "-1";
            size_t i;
            for (i = 0; i < cadena.size(); i++) {
                if (cadena[i] == "*") {
                    op = "*";
                    break;
                }
                else if (cadena[i] == "/") {
                    op = "/";
                    break;
                }
                else if (cadena[i] == "(") {
                    op = "(";
                    break;
                }
            }

            if (op == "*") {
                arbol.insertar("<T>", actual->izquierdo, actual);
                arbol.insertar("*", actual->medio, actual);
                arbol.insertar("<T>", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                actual = actual->izquierdo;
            }
            else if (op == "/") {
                arbol.insertar("<T>", actual->izquierdo, actual);
                arbol.insertar("/", actual->medio, actual);
                arbol.insertar("<T>", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                actual = actual->izquierdo;
            }
            else {
                arbol.insertar("NULL", actual->izquierdo, actual);
                arbol.insertar("<U>", actual->medio, actual);
                arbol.insertar("NULL", actual->derecho, actual);
                actual = actual->medio;
            }
        }
        else if (actual->valor == "<U>") {
            string op = "-1";
            size_t i;
            for (i = 0; i < cadena.size(); i++) {
                if (cadena[i] == "num") {
                    op = "num";
                    break;
                }
                else if (cadena[i] == "-") {
                    op = "-";
                    break;
                }
                else if (cadena[i] == "(") {
                    op = "(";
                    break;
                }
            }

            if (op == "(") {
                arbol.insertar("(", actual->izquierdo, actual);
                arbol.insertar("<E>", actual->medio, actual);
                arbol.insertar(")", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);
                for (size_t j = 0; j < cadena.size(); j++) {
                    if (cadena[j] == ")") {
                        cadena.erase(cadena.begin() + j);
                        break;
                    }
                }
                actual = actual->medio;
            }
            else if (op == "-") {
                arbol.insertar("-", actual->izquierdo, actual);
                arbol.insertar("<U>", actual->medio, actual);
                arbol.insertar("NULL", actual->derecho, actual);
                actual = actual->medio;
            }
            else if (op == "num") {
                arbol.insertar("NULL", actual->izquierdo, actual);
                arbol.insertar("num", actual->medio, actual);
                arbol.insertar("NULL", actual->derecho, actual);
                cadena.erase(cadena.begin() + i);

                if (valores.size() > 0) {
                    arbol.insertar(valores[0], actual->medio->medio);
                    arbol.insertar("NULL", actual->medio->izquierdo);
                    arbol.insertar("NULL", actual->medio->derecho);
                    valores.erase(valores.begin());
                    ultimos.push_back(actual->medio->medio); /*Guarda el ultimo nodo para
                    posteriormente recorrer mas facilmente el arbol de abajo hacia arriba*/
                }

                actual = actual->padre;
                while (actual != nullptr) {
                    if (actual->valor == "<T>" && actual->medio != nullptr && (actual->medio->valor == "*" || actual->medio->valor == "/") && actual->derecho->medio == nullptr) {
                        operaciones.push_back(actual->medio->valor); /*Guarda las operaciones en una pila*/
                        actual = actual->derecho;
                        break;
                    }
                    else if (actual->valor == "<E>" && actual->medio != nullptr && (actual->medio->valor == "+" || actual->medio->valor == "-") && actual->derecho->medio == nullptr) {
                        operaciones.push_back(actual->medio->valor); /*Guarda las operaciones en una pila*/
                        actual = actual->derecho;
                        break;
                    }
                    actual = actual->padre;
                }
            }
        }
    }
}

// Funcion prototipo de la sustitucion de valores especificos
void resolverOperacion(vector<string>&);

// Genera el lenguaje intermedio (Representacion interna)
void generarLenguaje() {
    int indice = 0;
    vector<string> num;
    vector<string> interno;

    cout << "Representacion Interna\n";
    // Guardar los valores de las hojas en el vector num
    for (int i = 0; i < ultimos.size(); i++) {
        cout << "t[" << indice << "] = " << ultimos[i]->valor << "\n";
        num.push_back("t[" + to_string(indice) + "]");
        interno.push_back("t[" + to_string(indice) + "] = " + ultimos[i]->valor);
        indice++;
    }

    // Generar las operaciones desde las hojas hacia la raíz
    while (!operaciones.empty()) {
        // Tomar los dos últimos elementos del vector num
        string operandoDerecho = num.back();
        num.pop_back();
        string operandoIzquierdo = num.back();
        num.pop_back();

        // Obtener la última operación
        string operacion = operaciones.back();
        operaciones.pop_back();

        // Generar el nuevo temporal para la operación
        string instruccion = "t[" + to_string(indice) + "] = " + operandoIzquierdo + " " + operacion + " " + operandoDerecho;
        interno.push_back(instruccion); /*Guarda los pasos para despues procesarlos*/
        cout << instruccion << "\n";

        // Guardar el nuevo temporal en num
        num.push_back("t[" + to_string(indice) + "]");
        indice++;
    }
    interno.push_back("t[" + to_string(indice) + "] = t[" + to_string(indice-1) + "]");
    cout << interno.back() << "\n\n";
    resolverOperacion(interno);
}

// Genera el lenguaje intermedio (Sustitucion de valores)
void resolverOperacion(vector<string>& operaciones) {
    int flotantes = 0;
    vector<float> t(operaciones.size());

    cout << "Sustitucion de Valores especificos\n";
    // Generar las operaciones desde las hojas hacia la raíz
    while (operaciones.size() > 1) {
        size_t indice = operaciones[0].find_first_of('[');
        int paso = operaciones[0][indice + 1] - '0';
        if (paso < ultimos.size()) {
            // Si el paso es solo guardar el numero hace la conversion de tipos
            string val = ultimos[paso]->valor;
            size_t punto = val.find_first_of('.');
            if (punto == string::npos) {
                size_t igual = operaciones[0].find_first_of('=');
                igual += 2;
                string temp = "";
                for (int i = igual; i < operaciones[0].size(); i++) {
                    temp += operaciones[0][i];
                }
                for (int j = 0; j < igual; j++) {
                    cout << operaciones[0][j];
                }
                cout << "to_int( " << temp << " )\n"; /*Si el numero no tiene punto decimal
                lo convierte a entero*/
                t[paso] = stof(temp);
            }
            else {
                size_t punto2 = val.find_first_of('.', punto + 1);
                if (punto2 != string::npos) {
                    errores(3); /*Si el numero tiene dos puntos manda error*/
                }
                else {
                    size_t igual = operaciones[0].find_first_of('=');
                    igual += 2;
                    string temp = "";
                    for (int i = igual; i < operaciones[0].size(); i++) {
                        temp += operaciones[0][i];
                    }
                    for (int j = 0; j < igual; j++) {
                        cout << operaciones[0][j];
                    }
                    cout << "to_float( " << temp << " )\n"; /*Si el numero tiene un solo 
                    punto decimal convierte a flotante*/
                    flotantes++;
                    t[paso] = stof(temp);
                }
            }
        }
        else {
            // Si el paso contiene una operacion la realiza
            size_t igual = operaciones[0].find_first_of('=');
            size_t posT = operaciones[0].find_first_of('t', igual + 1);
            posT += 2;
            size_t posT2 = operaciones[0].find_first_of('t', posT);
            posT2 += 2;
            int indice = operaciones[0][posT] - '0';
            int indice2 = operaciones[0][posT2] - '0';
            char op = operaciones[0][posT + 3];
            for (int j = 0; j < igual + 2; j++) {
                cout << operaciones[0][j];
            }
            cout << t[indice] << " " << op << " " << t[indice2] << "\n";

            // Manejo de operaciones
            switch(op) {
            case '+':
                t[paso] = t[indice] + t[indice2];
                break;
            case '-':
                t[paso] = t[indice] - t[indice2];
                break;
            case '*':
                t[paso] = t[indice] * t[indice2];
                break;
            case '/':
                if (t[indice2] == 0) {
                    errores(4);
                }
                else {
                    t[paso] = t[indice] / t[indice2];
                }
                break;
            }
        }
        
        operaciones.erase(operaciones.begin());
    }

    // Para el ultimo paso solo manda el resultado a raiz
    size_t igual = operaciones[0].find_first_of('=');
    size_t posT = operaciones[0].find_first_of('t', igual + 1);
    posT += 2;
    int indice = operaciones[0][posT] - '0';
    for (int j = 0; j < igual + 2; j++) {
        cout << operaciones[0][j];
    }
    if (flotantes > 0) {
        cout << "to_float( " << t[indice] << " )\n";
    }
    else {
        cout << "to_int( " << t[indice] << " )\n";
    }
    
}

// Manejador de errores
void errores(int error) {
    switch (error) {
    case 1:
        cout << "Error 1.- Caracter '" << cadena[0] << "' NO valido\n";
        exit(0);
        break;
    case 2:
        cout << "Error 2.- Posible ambiguedad en la cadena ingresada\n";
        exit(0);
        break;
    case 3:
        cout << "Error 3.- Tipo de dato incorrecto\n";
        exit(0);
        break;
    case 4:
        cout << "Error 4.- Se intento dividir entre 0\n";
        exit(0);
        break;
    default:
        cout << "Error no identificado\n";
        break;
    }
}

// Crear ventana y visulizar el arbol de parseo
void verArbol(int argc, char** argv) {
    // Inicializar OpenGL
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Arbol de Parseo");

    initOpenGL();

    glutDisplayFunc(display); // Función de visualización
    glutKeyboardFunc(teclado); // Función de manejo de teclado

    glutMainLoop(); // Ejecutar el bucle principal de OpenGL

}

// Función principal
int main(int argc, char** argv) {
    string entrada;
    vector <int> listaErrores;
    cout << "Ingrese la cadena: ";
    getline(cin, entrada);
    
    lexer(entrada); // Tokenizar cadena

    // Si no hay errores en la cadena imprime la tokenizacion
    cout << "Cadena Tokenizada: ";
    for (int i = 0; i < cadena.size(); i++) {
        cout << cadena[i] << " ";
    }
    cout << "\n\n";

    encontrarAmbiguedad();
    parser(); // Analizar y construir el árbol
    generarLenguaje();

    // Imprimir o no el arbol de parseo, si es que todo salio bien
    cout << "Ver el arbol de parseo? (S)(N) ";
    char respuesta;
    cin >> respuesta;
    switch (respuesta) {
    case 'S':
    case 's':
        verArbol(argc, argv);
        break;
    case 'N':
    case 'n':
        return 0;
    } 
}