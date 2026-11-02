import java.io.*;
import java.util.*;

// ==========================================
// 1. JERARQUÍA DE PROGRAMAS (POLIMORFISMO)
// ==========================================
abstract class ProgramaAcademico {
    private String nombrePrograma;

    public ProgramaAcademico(String nombrePrograma) {
        this.nombrePrograma = nombrePrograma;
    }

    public String getNombrePrograma() {
        return nombrePrograma;
    }

    public abstract boolean evaluarAprobacion(List<Double> notas);
}

class Curso extends ProgramaAcademico {
    public Curso() {
        super("Curso");
    }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        if (notas == null || notas.isEmpty()) return false;
        double suma = 0;
        for (double n : notas) suma += n;
        double promedio = suma / notas.size();
        return promedio >= 10.0;
    }
}

class Diplomado extends ProgramaAcademico {
    public Diplomado() {
        super("Diplomado");
    }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        if (notas == null || notas.isEmpty()) return false;
        double suma = 0;
        for (double n : notas) suma += n;
        double promedio = suma / notas.size();
        return promedio >= 14.0;
    }
}

class Bootcamp extends ProgramaAcademico {
    public Bootcamp() {
        super("Bootcamp");
    }

    @Override
    public boolean evaluarAprobacion(List<Double> notas) {
        if (notas == null || notas.isEmpty()) return false;
        for (double n : notas) {
            if (n < 14.0) return false; // Ninguna nota individual < 14
        }
        return true;
    }
}

// ==========================================
// 2. JERARQUÍA DE PERSONAS (HERENCIA Y ENCAPSULAMIENTO)
// ==========================================
abstract class Persona {
    private String cedula;
    private String nombreCompleto;
    private String correoElectronico;

    public Persona(String cedula, String nombreCompleto, String correoElectronico) {
        this.cedula = cedula;
        this.nombreCompleto = nombreCompleto;
        this.correoElectronico = correoElectronico;
    }

    public String getCedula() { return cedula; }
    public String getNombreCompleto() { return nombreCompleto; }
    public String getCorreoElectronico() { return correoElectronico; }

    public abstract String getRol();
}

class Alumno extends Persona {
    private ProgramaAcademico programaAcademico;
    private List<Double> notas;

    public Alumno(String cedula, String nombreCompleto, String correoElectronico, ProgramaAcademico programaAcademico) {
        super(cedula, nombreCompleto, correoElectronico);
        this.programaAcademico = programaAcademico;
        this.notas = new ArrayList<>();
    }

    public ProgramaAcademico getProgramaAcademico() { return programaAcademico; }
    public List<Double> getNotas() { return notas; }

    public boolean agregarNota(double nota) {
        if (notas.size() < 3) {
            notas.add(nota);
            return true;
        }
        return false;
    }

    public Double eliminarUltimaNota() {
        if (!notas.isEmpty()) {
            return notas.remove(notas.size() - 1);
        }
        return null;
    }

    public double calcularPromedio() {
        if (notas.isEmpty()) return 0.0;
        double suma = 0;
        for (double n : notas) suma += n;
        return suma / notas.size();
    }

    public boolean estaAprobado() {
        if (programaAcademico == null) return false;
        return programaAcademico.evaluarAprobacion(notas);
    }

    @Override
    public String getRol() {
        return "Alumno";
    }
}

class Profesor extends Persona {
    private String especialidad;
    private String materia;

    public Profesor(String cedula, String nombreCompleto, String correoElectronico, String especialidad, String materia) {
        super(cedula, nombreCompleto, correoElectronico);
        this.especialidad = especialidad;
        this.materia = materia;
    }

    public String getEspecialidad() { return especialidad; }
    public String getMateria() { return materia; }

    @Override
    public String getRol() {
        return "Profesor";
    }
}

// Clase auxiliar para la Pila (LIFO)
class AccionNota {
    private String cedulaAlumno;
    private double nota;

    public AccionNota(String cedulaAlumno, double nota) {
        this.cedulaAlumno = cedulaAlumno;
        this.nota = nota;
    }

    public String getCedulaAlumno() { return cedulaAlumno; }
    public double getNota() { return nota; }
}

// ==========================================
// 3. SISTEMA PRINCIPAL (SGA-DO EN JAVA)
// ==========================================
public class Main {
    private List<Alumno> alumnos;
    private List<Profesor> profesores;
    // Pila LIFO oficial con Deque
    private Deque<AccionNota> pilaAccionesNotas;
    private Scanner scanner;

    public Main() {
        alumnos = new ArrayList<>();
        profesores = new ArrayList<>();
        pilaAccionesNotas = new ArrayDeque<>();
        scanner = new Scanner(System.in);
        cargarDatos();
    }

    private ProgramaAcademico obtenerInstanciaPrograma(String tipo) {
        if (tipo == null) return null;
        switch (tipo.trim().toLowerCase()) {
            case "curso": return new Curso();
            case "diplomado": return new Diplomado();
            case "bootcamp": return new Bootcamp();
            default: return null;
        }
    }

    // --- MANEJO RIGUROSO DE EXCEPCIONES Y PERSISTENCIA ---
    public void cargarDatos() {
        alumnos.clear();
        profesores.clear();

        File fileAlumnos = new File("alumnos.txt");
        if (fileAlumnos.exists()) {
            try (BufferedReader br = new BufferedReader(new FileReader(fileAlumnos))) {
                String linea;
                while ((linea = br.readLine()) != null) {
                    linea = linea.trim();
                    if (linea.isEmpty()) continue;
                    String[] partes = linea.split(",");
                    if (partes.length >= 4) {
                        String cedula = partes[0].trim();
                        String nombre = partes[1].trim();
                        String correo = partes[2].trim();
                        String tipoProg = partes[3].trim();

                        ProgramaAcademico prog = obtenerInstanciaPrograma(tipoProg);
                        Alumno alumno = new Alumno(cedula, nombre, correo, prog);

                        // Cargar notas solo si existen
                        for (int i = 4; i < partes.length && i < 7; i++) {
                            try {
                                double val = Double.parseDouble(partes[i].trim());
                                if (alumno.getNotas().size() < 3) {
                                    alumno.agregarNota(val);
                                }
                            } catch (NumberFormatException ignored) {}
                        }
                        alumnos.add(alumno);
                    }
                }
            } catch (IOException e) {
                System.out.println(" [ERROR] Excepción al leer alumnos.txt: " + e.getMessage());
            }
        }

        File fileProf = new File("profesores.txt");
        if (fileProf.exists()) {
            try (BufferedReader br = new BufferedReader(new FileReader(fileProf))) {
                String linea;
                while ((linea = br.readLine()) != null) {
                    linea = linea.trim();
                    if (linea.isEmpty()) continue;
                    String[] partes = linea.split(",");
                    if (partes.length >= 5) {
                        profesores.add(new Profesor(
                            partes[0].trim(), partes[1].trim(), partes[2].trim(), partes[3].trim(), partes[4].trim()
                        ));
                    }
                }
            } catch (IOException e) {
                System.out.println(" [ERROR] Excepción al leer profesores.txt: " + e.getMessage());
            }
        }
    }

    public void guardarAlumnos() {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter("alumnos.txt"))) {
            for (Alumno a : alumnos) {
                String progNombre = (a.getProgramaAcademico() != null) ? a.getProgramaAcademico().getNombrePrograma() : "Curso";
                StringBuilder sb = new StringBuilder();
                sb.append(a.getCedula()).append(",")
                  .append(a.getNombreCompleto()).append(",")
                  .append(a.getCorreoElectronico()).append(",")
                  .append(progNombre);

                List<Double> notas = a.getNotas();
                if (!notas.isEmpty()) {
                    for (double n : notas) {
                        sb.append(",");
                        if (n == (long) n) sb.append((long) n);
                        else sb.append(n);
                    }
                }
                bw.write(sb.toString());
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(" [ERROR] Excepción al escribir alumnos.txt: " + e.getMessage());
        }
    }

    public void guardarProfesores() {
        try (BufferedWriter bw = new BufferedWriter(new FileWriter("profesores.txt"))) {
            for (Profesor p : profesores) {
                String linea = String.format("%s,%s,%s,%s,%s",
                    p.getCedula(), p.getNombreCompleto(), p.getCorreoElectronico(), p.getEspecialidad(), p.getMateria());
                bw.write(linea);
                bw.newLine();
            }
        } catch (IOException e) {
            System.out.println(" [ERROR] Excepción al escribir profesores.txt: " + e.getMessage());
        }
    }

    // --- OPCIONES DEL MENÚ CON VALIDACIONES ---
    public void registrarAlumno() {
        System.out.println("\n--- 1. Registrar Alumno ---");
        System.out.print("Cédula: "); 
        String cedula = scanner.nextLine().trim();

        // Validación de cédula repetida
        for (Alumno a : alumnos) {
            if (a.getCedula().equals(cedula)) {
                System.out.println(" [ERROR] Ya existe un alumno registrado con la cédula " + cedula + ".");
                return;
            }
        }

        System.out.print("Nombre Completo: "); 
        String nombre = scanner.nextLine().trim();
        System.out.print("Correo Electrónico: "); 
        String correo = scanner.nextLine().trim();

        ProgramaAcademico prog = null;
        while (prog == null) {
            System.out.println("Seleccione Tipo de Programa:");
            System.out.println("  1. Curso");
            System.out.println("  2. Diplomado");
            System.out.println("  3. Bootcamp");
            System.out.print("Ingrese opción (1/2/3): ");
            String opProg = scanner.nextLine().trim();

            if (opProg.equals("1")) prog = new Curso();
            else if (opProg.equals("2")) prog = new Diplomado();
            else if (opProg.equals("3")) prog = new Bootcamp();
            else System.out.println(" [ERROR] Opción inválida. Debe seleccionar 1, 2 o 3.");
        }

        alumnos.add(new Alumno(cedula, nombre, correo, prog));
        guardarAlumnos();
        System.out.println("¡Alumno " + nombre + " registrado con éxito!");
    }

    public void registrarProfesor() {
        System.out.println("\n--- 2. Registrar Profesor ---");
        System.out.print("Cédula: "); 
        String cedula = scanner.nextLine().trim();

        // Validación de cédula repetida para profesores
        for (Profesor p : profesores) {
            if (p.getCedula().equals(cedula)) {
                System.out.println(" [ERROR] Ya existe un profesor registrado con la cédula " + cedula + ".");
                return;
            }
        }

        System.out.print("Nombre Completo: "); String nombre = scanner.nextLine().trim();
        System.out.print("Correo Electrónico: "); String correo = scanner.nextLine().trim();
        System.out.print("Especialidad: "); String esp = scanner.nextLine().trim();
        System.out.print("Materia Asignada: "); String mat = scanner.nextLine().trim();

        profesores.add(new Profesor(cedula, nombre, correo, esp, mat));
        guardarProfesores();
        System.out.println("¡Profesor " + nombre + " registrado con éxito!");
    }

    public void registrarNotaAlumno() {
        System.out.println("\n--- 3. Registrar Notas a un Alumno ---");
        System.out.print("Ingrese la Cédula del Alumno: "); String cedula = scanner.nextLine().trim();

        Alumno alumno = null;
        for (Alumno a : alumnos) {
            if (a.getCedula().equals(cedula)) {
                alumno = a;
                break;
            }
        }

        if (alumno == null) {
            System.out.println(" [ERROR] Alumno no encontrado.");
            return;
        }

        if (alumno.getNotas().size() >= 3) {
            System.out.println(" [Aviso] El alumno ya posee las 3 notas registradas.");
            return;
        }

        try {
            System.out.print("Ingrese calificación para " + alumno.getNombreCompleto() + " (0-20): ");
            double nota = Double.parseDouble(scanner.nextLine().trim());
            if (nota >= 0 && nota <= 20) {
                alumno.agregarNota(nota);
                pilaAccionesNotas.push(new AccionNota(alumno.getCedula(), nota)); // LIFO Push
                guardarAlumnos();
                System.out.println(" Nota " + nota + " agregada correctamente.");
            } else {
                System.out.println(" [ERROR] La nota debe estar entre 0 y 20.");
            }
        } catch (NumberFormatException e) {
            System.out.println(" [ERROR] Debe ingresar un valor numérico válido.");
        }
    }

    public void deshacerUltimoRegistroNota() {
        System.out.println("\n--- 4. Deshacer Último Registro de Nota (LIFO) ---");
        if (pilaAccionesNotas.isEmpty()) {
            System.out.println(" [Aviso] No hay acciones recientes para deshacer.");
            return;
        }

        AccionNota ultimaAccion = pilaAccionesNotas.pop(); // LIFO Pop
        Alumno alumno = null;
        for (Alumno a : alumnos) {
            if (a.getCedula().equals(ultimaAccion.getCedulaAlumno())) {
                alumno = a;
                break;
            }
        }

        if (alumno != null && alumno.getNotas().contains(ultimaAccion.getNota())) {
            alumno.getNotas().remove(ultimaAccion.getNota());
            guardarAlumnos();
            System.out.println(" [ÉXITO - Deshacer] Se removió la nota " + ultimaAccion.getNota() + " del alumno " + alumno.getNombreCompleto());
        } else {
            System.out.println(" [ERROR] No se pudo sincronizar la acción deshacer.");
        }
    }

    public void generarColaCertificados() {
        System.out.println("\n--- 5. Generar Cola de Certificados (FIFO) ---");
        cargarDatos();

        // Cola FIFO oficial con Queue / ArrayDeque
        Queue<Alumno> colaGraduandos = new ArrayDeque<>();

        for (Alumno a : alumnos) {
            if (a.estaAprobado()) {
                colaGraduandos.offer(a); // FIFO Enqueue
            }
        }

        int totalGraduandos = colaGraduandos.size();

        try (BufferedWriter bw = new BufferedWriter(new FileWriter("certificados_pendientes.txt"))) {
            bw.write("========================================="); bw.newLine();
            bw.write("REPORTE DE CERTIFICADOS PENDIENTES"); bw.newLine();
            bw.write("========================================="); bw.newLine();
            bw.write("Total de graduandos en cola: " + totalGraduandos); bw.newLine();

            int contador = 1;
            while (!colaGraduandos.isEmpty()) {
                Alumno g = colaGraduandos.poll(); // FIFO Dequeue
                String progNombre = g.getProgramaAcademico().getNombrePrograma();
                double prom = Math.round(g.calcularPromedio() * 10.0) / 10.0;
                String reglaExtra = progNombre.equals("Bootcamp") ? " (Cumple regla de ninguna nota < 14)" : "";

                bw.write(String.format("%d. [%s] %s", contador, g.getCedula(), g.getNombreCompleto())); bw.newLine();
                bw.write("- Programa: " + progNombre); bw.newLine();
                bw.write("- Promedio Final: " + prom); bw.newLine();
                bw.write("- Estatus: APROBADO" + reglaExtra); bw.newLine();
                contador++;
            }

            bw.write("========================================="); bw.newLine();
            bw.write("- Fin del reporte - Generado por SGA-DO *"); bw.newLine();

            System.out.println(" [ÉXITO] Se procesó la cola con " + totalGraduandos + " graduandos.");
            System.out.println(" [ÉXITO] Reporte exportado a 'certificados_pendientes.txt'.");
        } catch (IOException e) {
            System.out.println(" [ERROR] Excepción al escribir certificados_pendientes.txt: " + e.getMessage());
        }
    }

    public void mostrarReporteGeneral() {
        System.out.println("\n==================================================");
        System.out.println("MOSTRAR REPORTE GENERAL SGA-DO (JAVA)");
        System.out.println("==================================================");
        cargarDatos();

        System.out.println("\n--- PROFESORES REGISTRADOS ---");
        if (profesores.isEmpty()) {
            System.out.println(" (Sin profesores registrados)");
        } else {
            for (Profesor p : profesores) {
                System.out.printf(" • [%s] %s | Esp: %s | Mat: %s\n",
                    p.getCedula(), p.getNombreCompleto(), p.getEspecialidad(), p.getMateria());
            }
        }

        System.out.println("\n--- ALUMNOS REGISTRADOS ---");
        if (alumnos.isEmpty()) {
            System.out.println(" (Sin alumnos registrados)");
        } else {
            for (Alumno a : alumnos) {
                String progNombre = (a.getProgramaAcademico() != null) ? a.getProgramaAcademico().getNombrePrograma() : "Sin Programa";
                double prom = Math.round(a.calcularPromedio() * 10.0) / 10.0;
                String estatus = a.estaAprobado() ? "APROBADO" : "EN PROCESO / REPROBADO";
                System.out.printf(" • [%s] %s | Prog: %s | Notas: %s | Prom: %.1f | Estatus: %s\n",
                    a.getCedula(), a.getNombreCompleto(), progNombre, a.getNotas(), prom, estatus);
            }
        }
    }

    public void ejecutarMenu() {
        while (true) {
            System.out.println("\n==================================================");
            System.out.println("SGA-DO: SISTEMA DIPLOMADOSONLINE (JAVA)");
            System.out.println("==================================================");
            System.out.println("1. Registrar Alumno");
            System.out.println("2. Registrar Profesor");
            System.out.println("3. Registrar Notas a un Alumno");
            System.out.println("4. Deshacer Último Registro de Nota");
            System.out.println("5. Generar Cola de Certificados");
            System.out.println("6. Mostrar Reporte General");
            System.out.println("7. Salir");
            System.out.println("==================================================");
            System.out.print("Seleccione una opción (1-7): ");

            String opcion = scanner.nextLine().trim();

            switch (opcion) {
                case "1": registrarAlumno(); break;
                case "2": registrarProfesor(); break;
                case "3": registrarNotaAlumno(); break;
                case "4": deshacerUltimoRegistroNota(); break;
                case "5": generarColaCertificados(); break;
                case "6": mostrarReporteGeneral(); break;
                case "7":
                    System.out.println("\n Guardando datos y cerrando el sistema de forma segura...");
                    guardarAlumnos();
                    guardarProfesores();
                    System.out.println("¡Hasta luego!");
                    return;
                default:
                    System.out.println(" [ERROR] Opción no válida. Ingrese un número entre 1 y 7.");
            }
        }
    }

    public static void main(String[] args) {
        Main sistema = new Main();
        sistema.ejecutarMenu();
    }
}