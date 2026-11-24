#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <cmath>

// ==========================================
// 1. JERARQUÍA DE PROGRAMAS (POLIMORFISMO)
// ==========================================
class ProgramaAcademico {
private:
    std::string nombrePrograma;

public:
    explicit ProgramaAcademico(const std::string& nombre) : nombrePrograma(nombre) {}
    virtual ~ProgramaAcademico() = default;

    std::string getNombrePrograma() const {
        return nombrePrograma;
    }

    // Regla general: requiere exactamente 3 notas y cumplir criterio propio
    virtual bool evaluarAprobacion(const std::vector<double>& notas) const = 0;
};

class Curso : public ProgramaAcademico {
public:
    Curso() : ProgramaAcademico("Curso") {}

    bool evaluarAprobacion(const std::vector<double>& notas) const override {
        if (notas.size() != 3) return false;
        double suma = 0.0;
        for (double n : notas) suma += n;
        double promedio = suma / 3.0;
        return promedio >= 10.0;
    }
};

class Diplomado : public ProgramaAcademico {
public:
    Diplomado() : ProgramaAcademico("Diplomado") {}

    bool evaluarAprobacion(const std::vector<double>& notas) const override {
        if (notas.size() != 3) return false;
        double suma = 0.0;
        for (double n : notas) suma += n;
        double promedio = suma / 3.0;
        return promedio >= 14.0;
    }
};

class Bootcamp : public ProgramaAcademico {
public:
    Bootcamp() : ProgramaAcademico("Bootcamp") {}

    bool evaluarAprobacion(const std::vector<double>& notas) const override {
        if (notas.size() != 3) return false;
        for (double n : notas) {
            if (n < 14.0) return false;
        }
        return true;
    }
};

// ==========================================
// 2. JERARQUÍA DE PERSONAS (ENCAPSULAMIENTO)
// ==========================================
class Persona {
private:
    std::string cedula;
    std::string nombreCompleto;
    std::string correoElectronico;

public:
    Persona(const std::string& ced, const std::string& nom, const std::string& corr)
        : cedula(ced), nombreCompleto(nom), correoElectronico(corr) {}

    virtual ~Persona() = default;

    std::string getCedula() const { return cedula; }
    std::string getNombreCompleto() const { return nombreCompleto; }
    std::string getCorreoElectronico() const { return correoElectronico; }

    virtual std::string getRol() const = 0;
};

class Alumno : public Persona {
private:
    ProgramaAcademico* programaAcademico; // Puntero gestionado manualmente
    std::vector<double> notas;

public:
    Alumno(const std::string& ced, const std::string& nom, const std::string& corr, ProgramaAcademico* prog)
        : Persona(ced, nom, corr), programaAcademico(prog) {}

    ~Alumno() override {
        delete programaAcademico;
        programaAcademico = nullptr;
    }

    ProgramaAcademico* getProgramaAcademico() const { return programaAcademico; }
    const std::vector<double>& getNotas() const { return notas; }
    std::vector<double>& getNotasModificables() { return notas; }

    bool agregarNota(double nota) {
        if (notas.size() < 3) {
            notas.push_back(nota);
            return true;
        }
        return false;
    }

    bool removerNotaEspecifica(double nota) {
        for (auto it = notas.begin(); it != notas.end(); ++it) {
            if (std::fabs(*it - nota) < 1e-9) {
                notas.erase(it);
                return true;
            }
        }
        return false;
    }

    double calcularPromedio() const {
        if (notas.empty()) return 0.0;
        double suma = 0.0;
        for (double n : notas) suma += n;
        return suma / static_cast<double>(notas.size());
    }

    bool estaAprobado() const {
        if (programaAcademico == nullptr) return false;
        return programaAcademico->evaluarAprobacion(notas);
    }

    std::string getRol() const override {
        return "Alumno";
    }
};

class Profesor : public Persona {
private:
    std::string especialidad;
    std::string materia;

public:
    Profesor(const std::string& ced, const std::string& nom, const std::string& corr,
             const std::string& esp, const std::string& mat)
        : Persona(ced, nom, corr), especialidad(esp), materia(mat) {}

    ~Profesor() override = default;

    std::string getEspecialidad() const { return especialidad; }
    std::string getMateria() const { return materia; }

    std::string getRol() const override {
        return "Profesor";
    }
};

struct AccionNota {
    std::string cedulaAlumno;
    double nota;
};

// ==========================================
// 3. SISTEMA PRINCIPAL SGA-DO (C++)
// ==========================================
class SistemaSGADO {
private:
    std::vector<Alumno*> alumnos;
    std::vector<Profesor*> profesores;
    std::stack<AccionNota*> pilaAccionesNotas; // Pila LIFO con punteros dinámicos

    static std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) return "";
        size_t last = str.find_last_not_of(" \t\r\n");
        return str.substr(first, last - first + 1);
    }

    static ProgramaAcademico* crearProgramaDesdeTexto(const std::string& tipo) {
        std::string lower = tipo;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        lower = trim(lower);

        if (lower == "curso") return new Curso();
        if (lower == "diplomado") return new Diplomado();
        if (lower == "bootcamp") return new Bootcamp();
        return nullptr;
    }

    void liberarMemoria() {
        for (Alumno* a : alumnos) {
            delete a;
        }
        alumnos.clear();

        for (Profesor* p : profesores) {
            delete p;
        }
        profesores.clear();

        while (!pilaAccionesNotas.empty()) {
            AccionNota* acc = pilaAccionesNotas.top();
            pilaAccionesNotas.pop();
            delete acc;
        }
    }

public:
    SistemaSGADO() {
        cargarDatos();
    }

    ~SistemaSGADO() {
        liberarMemoria();
    }

    void cargarDatos() {
        liberarMemoria();

        // Lectura de alumnos.txt con fstream
        std::ifstream archivoAlumnos("alumnos.txt");
        if (archivoAlumnos.is_open()) {
            std::string linea;
            while (std::getline(archivoAlumnos, linea)) {
                linea = trim(linea);
                if (linea.empty()) continue;

                std::stringstream ss(linea);
                std::string campo;
                std::vector<std::string> partes;
                while (std::getline(ss, campo, ',')) {
                    partes.push_back(trim(campo));
                }

                if (partes.size() >= 4) {
                    ProgramaAcademico* prog = crearProgramaDesdeTexto(partes[3]);
                    Alumno* alumno = new Alumno(partes[0], partes[1], partes[2], prog);

                    for (size_t i = 4; i < partes.size() && alumno->getNotas().size() < 3; ++i) {
                        if (partes[i].empty()) continue;
                        try {
                            double val = std::stod(partes[i]);
                            alumno->agregarNota(val);
                        } catch (...) {
                            // Ignora valores no numéricos corruptos en el archivo
                        }
                    }
                    alumnos.push_back(alumno);
                }
            }
            archivoAlumnos.close();
        }

        // Lectura de profesores.txt con fstream
        std::ifstream archivoProfesores("profesores.txt");
        if (archivoProfesores.is_open()) {
            std::string linea;
            while (std::getline(archivoProfesores, linea)) {
                linea = trim(linea);
                if (linea.empty()) continue;

                std::stringstream ss(linea);
                std::string campo;
                std::vector<std::string> partes;
                while (std::getline(ss, campo, ',')) {
                    partes.push_back(trim(campo));
                }

                if (partes.size() >= 5) {
                    Profesor* prof = new Profesor(partes[0], partes[1], partes[2], partes[3], partes[4]);
                    profesores.push_back(prof);
                }
            }
            archivoProfesores.close();
        }
    }

    void guardarAlumnos() const {
        std::ofstream archivo("alumnos.txt");
        if (!archivo.is_open()) {
            std::cout << " [ERROR] No se pudo abrir alumnos.txt para escritura.\n";
            return;
        }

        for (const Alumno* a : alumnos) {
            std::string progNombre = (a->getProgramaAcademico() != nullptr)
                ? a->getProgramaAcademico()->getNombrePrograma()
                : "Curso";

            archivo << a->getCedula() << ","
                    << a->getNombreCompleto() << ","
                    << a->getCorreoElectronico() << ","
                    << progNombre;

            for (double n : a->getNotas()) {
                archivo << ",";
                if (std::fabs(n - std::round(n)) < 1e-9) {
                    archivo << static_cast<long long>(std::round(n));
                } else {
                    archivo << n;
                }
            }
            archivo << "\n";
        }
        archivo.close();
    }

    void guardarProfesores() const {
        std::ofstream archivo("profesores.txt");
        if (!archivo.is_open()) {
            std::cout << " [ERROR] No se pudo abrir profesores.txt para escritura.\n";
            return;
        }

        for (const Profesor* p : profesores) {
            archivo << p->getCedula() << ","
                    << p->getNombreCompleto() << ","
                    << p->getCorreoElectronico() << ","
                    << p->getEspecialidad() << ","
                    << p->getMateria() << "\n";
        }
        archivo.close();
    }

    void registrarAlumno() {
        std::cout << "\n--- 1. Registrar Alumno ---\n";
        std::string cedula, nombre, correo;

        std::cout << "Cedula: ";
        std::getline(std::cin, cedula);
        cedula = trim(cedula);

        for (const Alumno* a : alumnos) {
            if (a->getCedula() == cedula) {
                std::cout << " [ERROR] Ya existe un alumno con la cedula " << cedula << ".\n";
                return;
            }
        }

        std::cout << "Nombre Completo: ";
        std::getline(std::cin, nombre);
        nombre = trim(nombre);

        std::cout << "Correo Electronico: ";
        std::getline(std::cin, correo);
        correo = trim(correo);

        ProgramaAcademico* prog = nullptr;
        while (prog == nullptr) {
            std::cout << "Seleccione Tipo de Programa:\n";
            std::cout << "  1. Curso\n";
            std::cout << "  2. Diplomado\n";
            std::cout << "  3. Bootcamp\n";
            std::cout << "Ingrese opcion (1/2/3): ";
            std::string op;
            std::getline(std::cin, op);
            op = trim(op);

            if (op == "1") prog = new Curso();
            else if (op == "2") prog = new Diplomado();
            else if (op == "3") prog = new Bootcamp();
            else std::cout << " [ERROR] Opcion invalida. Seleccione 1, 2 o 3.\n";
        }

        Alumno* nuevo = new Alumno(cedula, nombre, correo, prog);
        alumnos.push_back(nuevo);
        guardarAlumnos();
        std::cout << "¡Alumno " << nombre << " registrado con exito!\n";
    }

    void registrarProfesor() {
        std::cout << "\n--- 2. Registrar Profesor ---\n";
        std::string cedula, nombre, correo, esp, mat;

        std::cout << "Cedula: ";
        std::getline(std::cin, cedula);
        cedula = trim(cedula);

        for (const Profesor* p : profesores) {
            if (p->getCedula() == cedula) {
                std::cout << " [ERROR] Ya existe un profesor con la cedula " << cedula << ".\n";
                return;
            }
        }

        std::cout << "Nombre Completo: ";
        std::getline(std::cin, nombre);
        nombre = trim(nombre);

        std::cout << "Correo Electronico: ";
        std::getline(std::cin, correo);
        correo = trim(correo);

        std::cout << "Especialidad: ";
        std::getline(std::cin, esp);
        esp = trim(esp);

        std::cout << "Materia Asignada: ";
        std::getline(std::cin, mat);
        mat = trim(mat);

        Profesor* nuevo = new Profesor(cedula, nombre, correo, esp, mat);
        profesores.push_back(nuevo);
        guardarProfesores();
        std::cout << "¡Profesor " << nombre << " registrado con exito!\n";
    }

    void registrarNotaAlumno() {
        std::cout << "\n--- 3. Registrar Notas a un Alumno ---\n";
        std::cout << "Ingrese la Cedula del Alumno: ";
        std::string cedula;
        std::getline(std::cin, cedula);
        cedula = trim(cedula);

        Alumno* objetivo = nullptr;
        for (Alumno* a : alumnos) {
            if (a->getCedula() == cedula) {
                objetivo = a;
                break;
            }
        }

        if (objetivo == nullptr) {
            std::cout << " [ERROR] Alumno no encontrado.\n";
            return;
        }

        if (objetivo->getNotas().size() >= 3) {
            std::cout << " [Aviso] El alumno ya posee las 3 notas registradas.\n";
            return;
        }

        std::cout << "Ingrese calificacion para " << objetivo->getNombreCompleto() << " (0-20): ";
        std::string entrada;
        std::getline(std::cin, entrada);
        entrada = trim(entrada);

        try {
            double nota = std::stod(entrada);
            if (nota >= 0.0 && nota <= 20.0) {
                objetivo->agregarNota(nota);
                AccionNota* acc = new AccionNota{objetivo->getCedula(), nota};
                pilaAccionesNotas.push(acc);
                guardarAlumnos();
                std::cout << " Nota " << nota << " agregada correctamente.\n";
            } else {
                std::cout << " [ERROR] La nota debe estar entre 0 y 20.\n";
            }
        } catch (...) {
            std::cout << " [ERROR] Debe ingresar un valor numerico valido.\n";
        }
    }

    void deshacerUltimoRegistroNota() {
        std::cout << "\n--- 4. Deshacer Ultimo Registro de Nota (LIFO) ---\n";
        if (pilaAccionesNotas.empty()) {
            std::cout << " [Aviso] No hay acciones recientes para deshacer.\n";
            return;
        }

        AccionNota* ultima = pilaAccionesNotas.top();
        pilaAccionesNotas.pop();

        Alumno* objetivo = nullptr;
        for (Alumno* a : alumnos) {
            if (a->getCedula() == ultima->cedulaAlumno) {
                objetivo = a;
                break;
            }
        }

        if (objetivo != nullptr && objetivo->removerNotaEspecifica(ultima->nota)) {
            guardarAlumnos();
            std::cout << " [EXITO - Deshacer] Se removio la nota " << ultima->nota
                      << " del alumno " << objetivo->getNombreCompleto() << ".\n";
        } else {
            std::cout << " [ERROR] No se pudo sincronizar la accion deshacer.\n";
        }

        delete ultima;
    }

    void generarColaCertificados() {
        std::cout << "\n--- 5. Generar Cola de Certificados (FIFO) ---\n";
        cargarDatos();

        std::queue<Alumno*> colaGraduandos;
        for (Alumno* a : alumnos) {
            if (a->estaAprobado()) {
                colaGraduandos.push(a);
            }
        }

        int totalGraduandos = static_cast<int>(colaGraduandos.size());
        std::ofstream reporte("certificados_pendientes.txt");
        if (!reporte.is_open()) {
            std::cout << " [ERROR] No se pudo escribir certificados_pendientes.txt.\n";
            return;
        }

        reporte << "=========================================\n";
        reporte << "REPORTE DE CERTIFICADOS PENDIENTES\n";
        reporte << "=========================================\n";
        reporte << "Total de graduandos en cola: " << totalGraduandos << "\n";

        int contador = 1;
        while (!colaGraduandos.empty()) {
            Alumno* g = colaGraduandos.front();
            colaGraduandos.pop();

            std::string progNombre = g->getProgramaAcademico()->getNombrePrograma();
            double prom = std::round(g->calcularPromedio() * 10.0) / 10.0;
            std::string reglaExtra = (progNombre == "Bootcamp")
                ? " (Cumple regla de 3 notas y ninguna < 14)"
                : "";

            reporte << contador << ". [" << g->getCedula() << "] " << g->getNombreCompleto() << "\n";
            reporte << "- Programa: " << progNombre << "\n";
            reporte << "- Promedio Final: " << std::fixed << std::setprecision(1) << prom << "\n";
            reporte << "- Estatus: APROBADO" << reglaExtra << "\n";
            contador++;
        }

        reporte << "=========================================\n";
        reporte << "- Fin del reporte - Generado por SGA-DO *\n";
        reporte.close();

        std::cout << " [EXITO] Se proceso la cola con " << totalGraduandos << " graduandos.\n";
        std::cout << " [EXITO] Reporte exportado a 'certificados_pendientes.txt'.\n";
    }

    void mostrarReporteGeneral() {
        std::cout << "\n==================================================\n";
        std::cout << "MOSTRAR REPORTE GENERAL SGA-DO (C++)\n";
        std::cout << "==================================================\n";
        cargarDatos();

        std::cout << "\n--- PROFESORES REGISTRADOS ---\n";
        if (profesores.empty()) {
            std::cout << " (Sin profesores registrados)\n";
        } else {
            for (const Profesor* p : profesores) {
                std::cout << " • [" << p->getCedula() << "] " << p->getNombreCompleto()
                          << " | Esp: " << p->getEspecialidad()
                          << " | Mat: " << p->getMateria() << "\n";
            }
        }

        std::cout << "\n--- ALUMNOS REGISTRADOS ---\n";
        if (alumnos.empty()) {
            std::cout << " (Sin alumnos registrados)\n";
        } else {
            for (const Alumno* a : alumnos) {
                std::string progNombre = (a->getProgramaAcademico() != nullptr)
                    ? a->getProgramaAcademico()->getNombrePrograma()
                    : "Sin Programa";
                double prom = std::round(a->calcularPromedio() * 10.0) / 10.0;
                std::string estatus = a->estaAprobado() ? "APROBADO" : "EN PROCESO / REPROBADO";

                std::cout << " • [" << a->getCedula() << "] " << a->getNombreCompleto()
                          << " | Prog: " << progNombre
                          << " | Notas (" << a->getNotas().size() << "/3): [";
                for (size_t i = 0; i < a->getNotas().size(); ++i) {
                    std::cout << a->getNotas()[i];
                    if (i + 1 < a->getNotas().size()) std::cout << ", ";
                }
                std::cout << "] | Prom: " << std::fixed << std::setprecision(1) << prom
                          << " | Estatus: " << estatus << "\n";
            }
        }
    }

    void ejecutarMenu() {
        while (true) {
            std::cout << "\n==================================================\n";
            std::cout << "SGA-DO: SISTEMA DIPLOMADOSONLINE (C++)\n";
            std::cout << "==================================================\n";
            std::cout << "1. Registrar Alumno\n";
            std::cout << "2. Registrar Profesor\n";
            std::cout << "3. Registrar Notas a un Alumno\n";
            std::cout << "4. Deshacer Ultimo Registro de Nota\n";
            std::cout << "5. Generar Cola de Certificados\n";
            std::cout << "6. Mostrar Reporte General\n";
            std::cout << "7. Salir\n";
            std::cout << "==================================================\n";
            std::cout << "Seleccione una opcion (1-7): ";

            std::string opcion;
            if (!std::getline(std::cin, opcion)) break;
            opcion = trim(opcion);

            if (opcion == "1") registrarAlumno();
            else if (opcion == "2") registrarProfesor();
            else if (opcion == "3") registrarNotaAlumno();
            else if (opcion == "4") deshacerUltimoRegistroNota();
            else if (opcion == "5") generarColaCertificados();
            else if (opcion == "6") mostrarReporteGeneral();
            else if (opcion == "7") {
                std::cout << "\nGuardando datos y liberando memoria...\n";
                guardarAlumnos();
                guardarProfesores();
                liberarMemoria();
                std::cout << "¡Hasta luego!\n";
                break;
            } else {
                std::cout << " [ERROR] Opcion no valida. Ingrese un numero entre 1 y 7.\n";
            }
        }
    }
};

int main() {
    SistemaSGADO sistema;
    sistema.ejecutarMenu();
    return 0;
}