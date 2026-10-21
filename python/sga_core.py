import os
from abc import ABC, abstractmethod
from collections import deque

# ==========================================
# 1. JERARQUÍA DE PROGRAMAS (POLIMORFISMO)
# ==========================================
class ProgramaAcademico(ABC):
    def __init__(self, nombre_programa):
        self.nombre_programa = nombre_programa

    @abstractmethod
    def evaluar_aprobacion(self, notas):
        pass


class Curso(ProgramaAcademico):
    def __init__(self):
        super().__init__("Curso")

    def evaluar_aprobacion(self, notas):
        if not notas:
            return False
        promedio = sum(notas) / len(notas)
        return promedio >= 10.0


class Diplomado(ProgramaAcademico):
    def __init__(self):
        super().__init__("Diplomado")

    def evaluar_aprobacion(self, notas):
        if not notas:
            return False
        promedio = sum(notas) / len(notas)
        return promedio >= 14.0


class Bootcamp(ProgramaAcademico):
    def __init__(self):
        super().__init__("Bootcamp")

    def evaluar_aprobacion(self, notas):
        if not notas:
            return False
        # Ninguna nota individual puede ser menor a 14
        return all(nota >= 14.0 for nota in notas)


def obtener_instancia_programa(tipo_str):
    if not tipo_str:
        return None
    tipo = tipo_str.strip().lower()
    if tipo == "curso":
        return Curso()
    elif tipo == "diplomado":
        return Diplomado()
    elif tipo == "bootcamp":
        return Bootcamp()
    return None


# ==========================================
# 2. JERARQUÍA DE PERSONAS (HERENCIA)
# ==========================================
class Persona(ABC):
    def __init__(self, cedula, nombre_completo, correo_electronico):
        self._cedula = cedula
        self._nombre_completo = nombre_completo
        self._correo_electronico = correo_electronico

    @abstractmethod
    def get_rol(self):
        pass


class Alumno(Persona):
    def __init__(self, cedula, nombre_completo, correo_electronico, programa_academico):
        super().__init__(cedula, nombre_completo, correo_electronico)
        self.programa_academico = programa_academico
        self.notas = []

    def agregar_nota(self, nota):
        if len(self.notas) < 3:
            self.notas.append(nota)
            return True
        return False

    def eliminar_ultima_nota(self):
        if self.notas:
            return self.notas.pop()
        return None

    def calcular_promedio(self):
        if not self.notas:
            return 0.0
        return sum(self.notas) / len(self.notas)

    def esta_aprobado(self):
        if not self.programa_academico:
            return False
        return self.programa_academico.evaluar_aprobacion(self.notas)

    def get_rol(self):
        return "Alumno"


class Profesor(Persona):
    def __init__(self, cedula, nombre_completo, correo_electronico, especialidad, materia):
        super().__init__(cedula, nombre_completo, correo_electronico)
        self.especialidad = especialidad
        self.materia = materia

    def get_rol(self):
        return "Profesor"


# ==========================================
# 3. SISTEMA PRINCIPAL DE GESTIÓN (SGA-DO)
# ==========================================
class SistemaGestionSGA:
    def __init__(self):
        self.alumnos = []
        self.profesores = []
        # Pila LIFO para deshacer notas: elementos (cedula_alumno, nota_agregada)
        self.pila_acciones_notas = []
        self.cargar_datos()

    # --- PERSISTENCIA Y CARGA DE ARCHIVOS CORREGIDA ---
    def cargar_datos(self):
        self.alumnos.clear()
        self.profesores.clear()

        # Cargar Alumnos
        if os.path.exists("alumnos.txt"):
            with open("alumnos.txt", "r", encoding="utf-8") as f:
                for linea in f:
                    linea = linea.strip()
                    if not linea:
                        continue
                    partes = [p.strip() for p in linea.split(",")]
                    if len(partes) >= 4:
                        cedula, nombre, correo, tipo_prog = partes[:4]
                        programa_obj = obtener_instancia_programa(tipo_prog)
                        alumno = Alumno(cedula, nombre, correo, programa_obj)
                        
                        # Cargar únicamente notas reales si están presentes en el archivo
                        for n in partes[4:]:
                            try:
                                val = float(n)
                                if len(alumno.notas) < 3:
                                    alumno.notas.append(val)
                            except ValueError:
                                pass
                        self.alumnos.append(alumno)

        # Cargar Profesores
        if os.path.exists("profesores.txt"):
            with open("profesores.txt", "r", encoding="utf-8") as f:
                for linea in f:
                    linea = linea.strip()
                    if not linea:
                        continue
                    partes = [p.strip() for p in linea.split(",")]
                    if len(partes) >= 5:
                        cedula, nombre, correo, esp, mat = partes[:5]
                        profesor = Profesor(cedula, nombre, correo, esp, mat)
                        self.profesores.append(profesor)

    def guardar_alumnos(self):
        with open("alumnos.txt", "w", encoding="utf-8") as f:
            for a in self.alumnos:
                prog_nombre = a.programa_academico.nombre_programa if a.programa_academico else "Curso"
                linea = f"{a._cedula},{a._nombre_completo},{a._correo_electronico},{prog_nombre}"
                if a.notas:
                    notas_str = ",".join(str(int(n) if n.is_integer() else n) for n in a.notas)
                    linea += f",{notas_str}"
                f.write(linea + "\n")

    def guardar_profesores(self):
        with open("profesores.txt", "w", encoding="utf-8") as f:
            for p in self.profesores:
                f.write(f"{p._cedula},{p._nombre_completo},{p._correo_electronico},{p.especialidad},{p.materia}\n")

    # --- OPCIONES DEL MENÚ CON VALIDACIONES ---
    def registrar_alumno(self):
        print("\n--- 1. Registrar Alumno ---")
        cedula = input("Cédula: ").strip()
        
        # Validación de cédula repetida
        if any(a._cedula == cedula for a in self.alumnos):
            print(f" [ERROR] Ya existe un alumno registrado con la cédula {cedula}.")
            return

        nombre = input("Nombre Completo: ").strip()
        correo = input("Correo Electrónico: ").strip()
        
        # Validación estricta del tipo de programa (sin asumir por defecto)
        prog = None
        while prog is None:
            print("Seleccione Tipo de Programa:")
            print("  1. Curso")
            print("  2. Diplomado")
            print("  3. Bootcamp")
            op_prog = input("Ingrese opción (1/2/3): ").strip()
            
            if op_prog == "1":
                prog = Curso()
            elif op_prog == "2":
                prog = Diplomado()
            elif op_prog == "3":
                prog = Bootcamp()
            else:
                print(" [ERROR] Opción inválida. Debe seleccionar 1, 2 o 3.")

        nuevo_alumno = Alumno(cedula, nombre, correo, prog)
        self.alumnos.append(nuevo_alumno)
        self.guardar_alumnos()
        print(f"¡Alumno {nombre} registrado con éxito!")

    def registrar_profesor(self):
        print("\n--- 2. Registrar Profesor ---")
        cedula = input("Cédula: ").strip()
        
        # Validación de cédula repetida para profesores
        if any(p._cedula == cedula for p in self.profesores):
            print(f" [ERROR] Ya existe un profesor registrado con la cédula {cedula}.")
            return

        nombre = input("Nombre Completo: ").strip()
        correo = input("Correo Electrónico: ").strip()
        especialidad = input("Especialidad: ").strip()
        materia = input("Materia Asignada: ").strip()

        nuevo_profesor = Profesor(cedula, nombre, correo, especialidad, materia)
        self.profesores.append(nuevo_profesor)
        self.guardar_profesores()
        print(f"¡Profesor {nombre} registrado con éxito!")

    def registrar_nota_alumno(self):
        print("\n--- 3. Registrar Notas a un Alumno ---")
        cedula = input("Ingrese la Cédula del Alumno: ").strip()
        alumno = next((a for a in self.alumnos if a._cedula == cedula), None)

        if not alumno:
            print(" [ERROR] Alumno no encontrado.")
            return

        if len(alumno.notas) >= 3:
            print(" [Aviso] El alumno ya posee las 3 notas registradas.")
            return

        try:
            nota = float(input(f"Ingrese calificación para {alumno._nombre_completo} (0-20): "))
            if 0 <= nota <= 20:
                alumno.agregar_nota(nota)
                self.pila_acciones_notas.append((alumno._cedula, nota))
                self.guardar_alumnos()
                print(f" Nota {nota} agregada correctamente a {alumno._nombre_completo}.")
            else:
                print(" [ERROR] La nota debe estar entre 0 y 20.")
        except ValueError:
            print(" [ERROR] Debe ingresar un número válido.")

    def deshacer_ultimo_registro_nota(self):
        print("\n--- 4. Deshacer Último Registro de Nota (LIFO) ---")
        if not self.pila_acciones_notas:
            print(" [Aviso] No hay acciones recientes de registro de notas para deshacer.")
            return

        cedula, nota_removida = self.pila_acciones_notas.pop()
        alumno = next((a for a in self.alumnos if a._cedula == cedula), None)

        if alumno and nota_removida in alumno.notas:
            alumno.notas.remove(nota_removida)
            self.guardar_alumnos()
            print(f" [ÉXITO - Deshacer] Se removió la nota {nota_removida} del alumno {alumno._nombre_completo} ({cedula}).")
        else:
            print(" [ERROR] No se pudo sincronizar la acción deshacer con los registros actuales.")

    def generar_cola_certificados(self):
        print("\n--- 5. Generar Cola de Certificados (FIFO) ---")
        self.cargar_datos()

        cola_graduandos = deque()

        for alumno in self.alumnos:
            if alumno.esta_aprobado():
                cola_graduandos.append(alumno)

        total_graduandos = len(cola_graduandos)

        with open("certificados_pendientes.txt", "w", encoding="utf-8") as f:
            f.write("=========================================\n")
            f.write("REPORTE DE CERTIFICADOS PENDIENTES\n")
            f.write("=========================================\n")
            f.write(f"Total de graduandos en cola: {total_graduandos}\n")

            contador = 1
            while cola_graduandos:
                graduando = cola_graduandos.popleft()
                prog_nombre = graduando.programa_academico.nombre_programa
                prom = round(graduando.calcular_promedio(), 1)
                
                regla_extra = ""
                if prog_nombre == "Bootcamp":
                    regla_extra = " (Cumple regla de ninguna nota < 14)"

                f.write(f"{contador}. [{graduando._cedula}] {graduando._nombre_completo}\n")
                f.write(f"- Programa: {prog_nombre}\n")
                f.write(f"- Promedio Final: {prom}\n")
                f.write(f"- Estatus: APROBADO{regla_extra}\n")
                contador += 1

            f.write("=========================================\n")
            f.write("- Fin del reporte - Generado por SGA-DO *\n")

        print(f" [ÉXITO] Se procesó la cola con {total_graduandos} graduandos.")
        print(" [ÉXITO] Reporte exportado a 'certificados_pendientes.txt'.")

    def mostrar_reporte_general(self):
        print("\n==================================================")
        print("MOSTRAR REPORTE GENERAL SGA-DO")
        print("==================================================")
        self.cargar_datos()

        print("\n--- PROFESORES REGISTRADOS ---")
        if not self.profesores:
            print(" (Sin profesores registrados)")
        else:
            for p in self.profesores:
                print(f" • [{p._cedula}] {p._nombre_completo} | Esp: {p.especialidad} | Mat: {p.materia}")

        print("\n--- ALUMNOS REGISTRADOS ---")
        if not self.alumnos:
            print(" (Sin alumnos registrados)")
        else:
            for a in self.alumnos:
                prog_nombre = a.programa_academico.nombre_programa if a.programa_academico else "Sin Programa"
                prom = round(a.calcular_promedio(), 1)
                estatus = "APROBADO" if a.esta_aprobado() else "EN PROCESO / REPROBADO"
                print(f" • [{a._cedula}] {a._nombre_completo} | Prog: {prog_nombre} | Notas: {a.notas} | Prom: {prom} | Estatus: {estatus}")

    def ejecutar_menu(self):
        while True:
            print("\n==================================================")
            print("SGA-DO: SISTEMA DIPLOMADOSONLINE")
            print("==================================================")
            print("1. Registrar Alumno")
            print("2. Registrar Profesor")
            print("3. Registrar Notas a un Alumno")
            print("4. Deshacer Último Registro de Nota")
            print("5. Generar Cola de Certificados")
            print("6. Mostrar Reporte General")
            print("7. Salir")
            print("==================================================")
            
            opcion = input("Seleccione una opción (1-7): ").strip()

            if opcion == "1":
                self.registrar_alumno()
            elif opcion == "2":
                self.registrar_profesor()
            elif opcion == "3":
                self.registrar_nota_alumno()
            elif opcion == "4":
                self.deshacer_ultimo_registro_nota()
            elif opcion == "5":
                self.generar_cola_certificados()
            elif opcion == "6":
                self.mostrar_reporte_general()
            elif opcion == "7":
                print("\n Guardando datos y cerrando el sistema de forma segura...")
                self.guardar_alumnos()
                self.guardar_profesores()
                print("¡Hasta luego!")
                break
            else:
                print(" [ERROR] Opción no válida. Ingrese un número entre 1 y 7.")


if __name__ == "__main__":
    sistema = SistemaGestionSGA()
    sistema.ejecutar_menu()