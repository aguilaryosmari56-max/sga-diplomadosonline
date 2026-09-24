# Especificación de Arquitectura de Clases UML

## Jerarquías Implementadas
1. **Jerarquía de Personas (Herencia):**
   * Class `Persona` (Abstracta) -> `#cedula`, `#nombreCompleto`, `#correoElectronico`.
   * Class `Alumno` -> Hereda de `Persona`. Atributos: `-listaDeNotas`, `-programaAsignado`.
   * Class `Profesor` -> Hereda de `Persona`. Atributos: `-especialidadAcademica`, `-materiaAsignada`.

2. **Jerarquía de Programas (Polimorfismo):**
   * Class `ProgramaAcademico` (Abstracta) -> Define método polimórfico `evaluarAprobacion()`.
   * Class `Curso` -> Aprueba si Promedio >= 10.
   * Class `Diplomado` -> Aprueba si Promedio >= 14.
   * Class `Bootcamp` -> Aprueba si ninguna nota individual es < 14.