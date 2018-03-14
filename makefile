test: new_tracegen p1 tracefiles
	mkdir results
	@echo "RUNNING NO PREDICTOR"
	@echo "User Test one - Data Hazards"
	./p1 tests/test_datahaz.tr > results/datahazards_none.txt 0 1
	@echo "Test two - Structural Hazards"
	./p1 tests/test_structhaz.tr 0 1 > results/structhazards_none.txt
	@echo "Test three - Control Hazards"
	./p1 tests/test_ctrlhaz.tr 0 1 > results/ctrlhazards_none.txt
	@echo "Test four - Structural + Data Hazards"
	./p1 tests/test_structdatahaz.tr 0 1 > results/struct_data_hazards_none.txt
	@echo "Test five - Data + Control Hazards"
	./p1 tests/test_datactrlhaz.tr 0 1 > results/data_cntrl_hazards_none.txt
	@echo "Test six - Control + Structural Hazards"
	./p1 tests/test_ctrlstructhaz.tr 0 1 > results/ctrl_struct_hazards_none.txt
	@echo "Test seven - Data + Control + Structural Hazards"
	./p1 tests/test_allhaz.tr 0 1 > results/all_hazards_none.txt
	@echo "Test eight - Control (unsquashed) Hazards"
	./p1 tests/test_ctrl_us.tr 0 1 > results/ctrl_hazards_us_none.txt
	@echo "Test nine - Random"
	./p1 tests/test_random1.tr 0 1 > results/random_none.txt
	@echo "Test ten - All R"
	./p1 tests/test_R.tr 0 1 > results/R_none.txt
	@echo "Test eleven - All L"
	./p1 tests/test_L.tr 0 1 > results/L_none.txt
	@echo "Test twelve - All S"
	./p1 tests/test_S.tr 0 1 > results/S_none.txt
	@echo "Test thirteen - All B"
	./p1 tests/test_B.tr 0 1 > results/B_none.txt
	
	@echo "Running samples"
	@echo "Large 1"
	./p1 /afs/cs.pitt.edu/courses/1541/long_traces/sample_large1.tr 0 0
	@echo "Large 2"
	./p1 /afs/cs.pitt.edu/courses/1541/long_traces/sample_large2.tr 0 0
	@echo "Small 0"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 0 0
	@echo "Small 1"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample1.tr 0 0
	@echo "Small 2"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample2.tr 0 0
	@echo "Small 3"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample3.tr 0 0
	@echo "Small 4"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample4.tr 0 0
	
	@echo "RUNNING 1-BIT PREDICTOR"
	@echo "User Test one - Data Hazards"
	./p1 tests/test_datahaz.tr 1 1 > results/datahazards_1bit.txt
	@echo "Test two - Structural Hazards"
	./p1 tests/test_structhaz.tr 1 1 > results/structhazards_1bit.txt
	@echo "Test three - Control Hazards"
	./p1 tests/test_ctrlhaz.tr 1 1 > results/ctrlhazards_1bit.txt
	@echo "Test four - Structural + Data Hazards"
	./p1 tests/test_structdatahaz.tr 1 1 > results/struct_data_hazards_1bit.txt
	@echo "Test five - Data + Control Hazards"
	./p1 tests/test_datactrlhaz.tr 1 1 > results/data_cntrl_hazards_1bit.txt
	@echo "Test six - Control + Structural Hazards"
	./p1 tests/test_ctrlstructhaz.tr 1 1 > results/ctrl_struct_hazards_1bit.txt
	@echo "Test seven - Data + Control + Structural Hazards"
	./p1 tests/test_allhaz.tr 1 1 > results/all_hazards_1bit.txt
	@echo "Test eight - Control (unsquashed) Hazards"
	./p1 tests/test_ctrl_us.tr 1 1 > results/ctrl_hazards_us_1bit.txt
	@echo "Test nine - Random"
	./p1 tests/test_random1.tr 1 1 > results/random_1bit.txt
	@echo "Test ten - All R"
	./p1 tests/test_R.tr 1 1 > results/R_1bit.txt
	@echo "Test eleven - All L"
	./p1 tests/test_L.tr 1 1 > results/L_1bit.txt
	@echo "Test twelve - All S"
	./p1 tests/test_S.tr 1 1 > results/S_1bit.txt
	@echo "Test thirteen - All B"
	./p1 tests/test_B.tr 1 1 > results/B_1bit.txt
	
	@echo "Running samples"
	@echo "Large 1"
	./p1 /afs/cs.pitt.edu/courses/1541/long_traces/sample_large1.tr 1 0
	@echo "Large 2"
	./p1 /afs/cs.pitt.edu/courses/1541/long_traces/sample_large2.tr 1 0
	@echo "Small 0"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 1 0
	@echo "Small 1"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample1.tr 1 0
	@echo "Small 2"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample2.tr 1 0
	@echo "Small 3"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample3.tr 1 0
	@echo "Small 4"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample4.tr 1 0
	
	@echo "RUNNING 2-BIT PREDICTOR"
	@echo "User Test one - Data Hazards"
	./p1 tests/test_datahaz.tr 2 1 > results/datahazards_2bit.txt
	@echo "Test two - Structural Hazards"
	./p1 tests/test_structhaz.tr 2 1 > results/structhazards_2bit.txt
	@echo "Test three - Control Hazards"
	./p1 tests/test_ctrlhaz.tr 2 1 > results/ctrlhazards_2bit.txt
	@echo "Test four - Structural + Data Hazards"
	./p1 tests/test_structdatahaz.tr 2 1 > results/struct_data_hazards_2bit.txt
	@echo "Test five - Data + Control Hazards"
	./p1 tests/test_datactrlhaz.tr 2 1 > results/data_cntrl_hazards_2bit.txt
	@echo "Test six - Control + Structural Hazards"
	./p1 tests/test_ctrlstructhaz.tr 2 1 > results/ctrl_struct_hazards_2bit.txt
	@echo "Test seven - Data + Control + Structural Hazards"
	./p1 tests/test_allhaz.tr 2 1 > results/all_hazards_2bit.txt
	@echo "Test eight - Control (unsquashed) Hazards"
	./p1 tests/test_ctrl_us.tr 2 1 > results/ctrl_hazards_us_2bit.txt
	@echo "Test nine - Random"
	./p1 tests/test_random1.tr 2 1 > results/random_2bit.txt
	@echo "Test ten - All R"
	./p1 tests/test_R.tr 2 1 > results/R_2bit.txt
	@echo "Test eleven - All L"
	./p1 tests/test_L.tr 2 1 > results/L_2bit.txt
	@echo "Test twelve - All S"
	./p1 tests/test_S.tr 2 1 > results/S_2bit.txt
	@echo "Test thirteen - All B"
	./p1 tests/test_B.tr 2 1 > results/B_2bit.txt
	
	@echo "Running samples"
	@echo "Large 1"
	./p1 /afs/cs.pitt.edu/courses/1541/long_traces/sample_large1.tr 2 0
	@echo "Large 2"
	./p1 /afs/cs.pitt.edu/courses/1541/long_traces/sample_large2.tr 2 0
	@echo "Small 0"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample.tr 2 0
	@echo "Small 1"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample1.tr 2 0
	@echo "Small 2"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample2.tr 2 0
	@echo "Small 3"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample3.tr 2 0
	@echo "Small 4"
	./p1 /afs/cs.pitt.edu/courses/1541/short_traces/sample4.tr 2 0
	

clean:
	rm -rf tests
	rm -rf results
	rm new_tracegen
	rm p1

new_tracegen:
	gcc -o new_tracegen trace_generator_modified.c
	
p1:
	gcc -o p1 pipeline.c
	
tracefiles:
	mkdir tests
	./new_tracegen tests/test_random1.tr 1
	./new_tracegen tests/test_datahaz.tr 2
	./new_tracegen tests/test_structhaz.tr 3
	./new_tracegen tests/test_ctrlhaz.tr 4
	./new_tracegen tests/test_structdatahaz.tr 5
	./new_tracegen tests/test_datactrlhaz.tr 6
	./new_tracegen tests/test_ctrlstructhaz.tr 7
	./new_tracegen tests/test_allhaz.tr 8
	./new_tracegen tests/test_ctrl_us.tr 9
	./new_tracegen tests/test_R.tr 10
	./new_tracegen tests/test_L.tr 11
	./new_tracegen tests/test_S.tr 12
	./new_tracegen tests/test_B.tr 13