func @test(%x : memref<?x?x?xf32>, %y : memref<?x?xf32>) {
	%c0 = constant 0: index
	%n = memref.dim %x, %c0 : memref<?x?x?xf32>
	%c1 = constant 1: index
	%m = memref.dim %x, %c1 : memref<?x?x?xf32>
	%c2 = constant 2: index
	%k = memref.dim %x, %c2 : memref<?x?x?xf32>


	%c3 = constant 0: index
	%p = memref.dim %y, %c3 : memref<?x?xf32>
	%c4 = constant 1: index
	%q = memref.dim %y, %c4 : memref<?x?xf32>

	return
}
