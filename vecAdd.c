/*
 * 2017 (c) Sebastian Jennen
 * Dies ist ein Testkernel zur Veranschaulichung der Funktionsweise von OpenCL.
 * Zu Beginn werden zwei Arrays A und B definiert. Dann wird OpenCL initialisiert
 * (1. Platform, 1. Gerät) und auf diesem mittels OpenCL die Vektoraddition der beiden
 * Arrays ausgeführt und in ein drittes Array zurückkopiert.
 */

#include <CL/cl.hpp>
#include <iostream>

// Fehlerausgabe und Beendigung des Programms
void errorExit(std::string errorText)
{
	std::cout << errorText << "\n";
	exit(1);
}

int main()
{
	// Host Variablen zum Test des Kernels
	int A[] = {3, 3, 3, 3, 3, 3, 4, 4};
	int B[] = {1, 2, 3, 4, 5, 6, 7, 8};
	const char SIZE = 8; // Größe der Buffer

	// Verfügbare Platformen bestimmen (AMD, Nvidia, ...)
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);
	if (platforms.size() == 0) errorExit("Keine Platformen verfügbar!");

	// Erste verfügbare Platform zum Test auswählen
	cl::Platform platform = platforms[0];
	std::cout << "Benutze Platform: " << platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	// Verfügbare Geräte bestimmen (jede einzelne Grafikkarte)
	std::vector<cl::Device> devices;
	platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
	if (devices.size() == 0) errorExit("Keine Geräte verfügbar!");

	// Erstes Gerät (egal welcher Art) von dieser Platform auswählen
	cl::Device device = devices[0];
	std::cout << "Benutze Gerät: " << device.getInfo<CL_DEVICE_NAME>() << "\n\n";

	// Erzeugung eines ausführbaren Kontextes für den Kernel
	cl::Context context({device});

	// Dies ist der OpenCL Quellcode der schlussendlich auf dem Gerät ausgeführt wird.
	// Dieser wird als String vorbereitet und enthält den Kernel vecAdd(...) als Beispiel für einen
	// simplen Kernel.
	// get_global_id(0) gibt die aktuelle Position der Range zurueck.
	// im Hostcode waere dies beispielsweise eine Laufvariable i.
	std::string kernelCode =
		"   void kernel vecAdd(global const int* A, global const int* B, global int* C){    "
		"       C[get_global_id(0)] = A[get_global_id(0)] + B[get_global_id(0)];            "
		"   }                                                                               ";

	// Erzeugung eines source Elements zum Laden und Kompilieren des OpenCL Quellcodes
	cl::Program::Sources sources;
	sources.push_back({kernelCode.c_str(), kernelCode.length()});

	// Erzeugung / Kompilierung des Programms aus dem Quelltext im optimiert für den Worker
	cl::Program program(context, sources);
	if (program.build({device}) != CL_SUCCESS)
		errorExit("Kann nicht kompilieren: " + program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(device));

	// Nötige Buffer auf dem Gerät für die Ein- und Ausgabe erzeugen
	cl::Buffer bufferA(context, CL_MEM_WRITE_ONLY, sizeof(int) * SIZE);
	cl::Buffer bufferB(context, CL_MEM_WRITE_ONLY, sizeof(int) * SIZE);
	cl::Buffer bufferC(context, CL_MEM_READ_ONLY, sizeof(int) * SIZE);

	// Erzeugung einer Queue zum Schreiben der Buffer
	cl::CommandQueue queue(context, device);

	// Eingabe Arrays A und B in die Queue zur Weitergabe an den Worker schreiben
	queue.enqueueWriteBuffer(bufferA, CL_TRUE, 0, sizeof(int) * SIZE, A);
	queue.enqueueWriteBuffer(bufferB, CL_TRUE, 0, sizeof(int) * SIZE, B);

	// Kernel als Zielfunktion festlegen
	cl::Kernel kernelVecAdd = cl::Kernel(program, "vecAdd");

	// Buffer als Argumente der Kernelfunktion festlegen
	kernelVecAdd.setArg(0, bufferA);
	kernelVecAdd.setArg(1, bufferB);
	kernelVecAdd.setArg(2, bufferC);

	// Über den Bereich von "SIZE" den Worker anweisen auf seinen übergebenen Buffern den Kernel
	// anzuwenden
	// Mit dem enqueue und dem anschließenden finish wird das Programm auf der GPU ausgeführt und
	// synchron auf das Ergebnis gewartet.
	queue.enqueueNDRangeKernel(kernelVecAdd, cl::NullRange, cl::NDRange(SIZE), cl::NullRange);
	queue.finish();

	// Ausgabe des Kernels vom Worker in den Host in die Variable C kopieren
	int C[SIZE];
	queue.enqueueReadBuffer(bufferC, CL_TRUE, 0, sizeof(int) * SIZE, C);

	// Ausgabe zur Verifizierung des korrekten Ergebnisses
	std::cout << "Ergebnis des vecAdd Kernels (Berechnet A + B = [C]): \n";
	for (int i = 0; i < SIZE; i++)
	{
		std::cout << A[i] << " + " << B[i] << " = " << C[i] << "\n";
	}
	return 0;
}
