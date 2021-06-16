#PNETCDF_DIR=/ccs/home/sklasky/parallel-netcdf/jaguar

all: write_2d_bp read_2d_bp write_2d_nc4 read_2d_nc4 write_2d_nc5 read_2d_nc5

read_2d_bp: read_2d_bp.F90
	ftn read_2d_bp.F90 -o read_2d_bp -L/ccs/home/ge1/work/jaguar/lib -ladiosreadf

read_2d_nc4: read_2d_nc4.F90
	ftn read_2d_nc4.F90 -o read_2d_nc4 -I$(NETCDF_DIR)/include -L$(NETCDF_DIR)/lib -lnetcdf

read_2d_nc5: read_2d_nc5.c
	cc read_2d_nc5.c -o read_2d_nc5 $(PNETCDF_LIB)
#	mpicc read_2d_nc5.c -o read_2d_nc5 $(PNETCDF_LIB)
#	cc read_2d_nc5.c -o read_2d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf
#	mpicc read_2d_nc5.c -o read_2d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf

write_2d_bp: write_2d_bp.F90 2d.xml
	python $(ADIOS_DIR)/bin/gpp.py 2d.xml
	ftn write_2d_bp.F90 -o write_2d_bp $(ADIOS_INC) $(ADIOS_FLIB)

write_2d_nc4: write_2d_nc4.F90
	ftn write_2d_nc4.F90 -o write_2d_nc4 -I$(NETCDF_DIR)/include -L$(NETCDF_DIR)/lib -lnetcdf

write_2d_nc5: write_2d_nc5.c
	cc write_2d_nc5.c -o write_2d_nc5 $(PNETCDF_LIB)
#	mpicc write_2d_nc5.c -o write_2d_nc5 $(PNETCDF_LIB)
#	cc write_2d_nc5.c -o write_2d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf
#	mpicc write_2d_nc5.c -o write_2d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf

clean:
	rm -f *.o *.mod *.fh *.ch read_2d_bp read_2d_nc4 read_2d_nc5 write_2d_bp write_2d_nc4 write_2d_nc5

install:
	cp read_2d_bp read_2d_nc4 read_2d_nc5 write_2d_bp write_2d_nc4 write_2d_nc5 2d.xml /tmp/work/ge1/a/2d
