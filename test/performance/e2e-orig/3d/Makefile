#PNETCDF_DIR=/ccs/home/sklasky/parallel-netcdf/jaguar

all: write_3d_bp write_3d_nc4 write_3d_nc5 read_3d_bp read_3d_nc4 read_3d_nc5

write_3d_bp: write_3d_bp.c 3d.xml
	python $(ADIOS_DIR)/bin/gpp.py 3d.xml
	cc write_3d_bp.c -o write_3d_bp $(ADIOS_INC) $(ADIOS_CLIB)

write_3d_nc4: write_3d_nc4.c
	cc write_3d_nc4.c -o write_3d_nc4 -I$(NETCDF_DIR)/include -L$(NETCDF_DIR)/lib -lnetcdf

write_3d_nc5: write_3d_nc5.c
	cc write_3d_nc5.c -o write_3d_nc5 $(PNETCDF_LIB)
#	cc write_3d_nc5.c -o write_3d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf

read_3d_bp: read_3d_bp.F90
	ftn read_3d_bp.F90 -o read_3d_bp $(ADIOSREAD_FLIB)

read_3d_nc4: read_3d_nc4.F90
	ftn read_3d_nc4.F90 -o read_3d_nc4 -I$(NETCDF_DIR)/include -L$(NETCDF_DIR)/lib -lnetcdf

read_3d_nc5: read_3d_nc5.c
	cc read_3d_nc5.c -o read_3d_nc5 $(PNETCDF_LIB)
#       mpicc read_3d_nc5.c -o read_3d_nc5 $(PNETCDF_LIB)
#	cc read_3d_nc5.c -o read_3d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf
#       mpicc read_3d_nc5.c -o read_3d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf

clean:
	rm -f write_3d_bp write_3d_nc4 write_3d_nc5 read_3d_bp read_3d_nc4 read_3d_nc5 *.mod *.ch

install:
	cp write_3d_bp write_3d_nc4 write_3d_nc5 read_3d_bp read_3d_nc4 read_3d_nc5 3d.xml /tmp/work/ge1/a/3d
	cp write_3d_bp 3d.xml /tmp/work/ge1/a/2d
	cp write_3d_bp 3d.xml /tmp/work/ge1/a/1d
