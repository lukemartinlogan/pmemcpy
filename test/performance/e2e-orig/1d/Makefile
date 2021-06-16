#PNETCDF_DIR=/ccs/home/sklasky/parallel-netcdf/jaguar

all: write_1d_bp write_1d_nc4 write_1d_nc5 read_1d_bp read_1d_nc4 read_1d_nc5

read_1d_bp: read_1d_bp.c
	cc read_1d_bp.c -o read_1d_bp $(ADIOS_INC) $(ADIOSREAD_CLIB)

read_1d_nc4: read_1d_nc4.c
	cc read_1d_nc4.c -o read_1d_nc4 -I$(NETCDF_DIR)/include -L$(NETCDF_DIR)/lib -lnetcdf

read_1d_nc5: read_1d_nc5.c
	cc read_1d_nc5.c -o read_1d_nc5 $(PNETCDF_LIB)
#	mpicc read_1d_nc5.c -o read_1d_nc5 $(PNETCDF_LIB)
#	cc read_1d_nc5.c -o read_1d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf
#	mpicc read_1d_nc5.c -o read_1d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf

write_1d_bp: write_1d_bp.c 1d.xml
	python $(ADIOS_DIR)/bin/gpp.py 1d.xml
	cc write_1d_bp.c -o write_1d_bp $(ADIOS_INC) $(ADIOS_CLIB)

write_1d_nc4: write_1d_nc4.c
	cc write_1d_nc4.c -o write_1d_nc4 -I$(NETCDF_DIR)/include -L$(NETCDF_DIR)/lib -lnetcdf

write_1d_nc5: write_1d_nc5.c
	cc write_1d_nc5.c -o write_1d_nc5 $(PNETCDF_LIB)
#	mpicc write_1d_nc5.c -o write_1d_nc5 $(PNETCDF_LIB)
#	cc write_1d_nc5.c -o write_1d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf
#	mpicc write_1d_nc5.c -o write_1d_nc5 -I$(PNETCDF_DIR)/include -L$(PNETCDF_DIR)/lib -lpnetcdf

clean:
	rm -f *.o *.mod *.fh *.ch read_1d_bp read_1d_nc4 read_1d_nc5 write_1d_bp write_1d_nc4 write_1d_nc5

install:
	cp read_1d_bp read_1d_nc4 read_1d_nc5 write_1d_bp write_1d_nc4 write_1d_nc5 1d.xml /tmp/work/ge1/a/1d
