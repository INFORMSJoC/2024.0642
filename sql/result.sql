create table optresults
(
    -- INSTANCE VALUES
    ifname          varchar(32),    -- a string tagging the instance
    -- OPT ALG INFO
    algorithm       varchar(16),    -- a string describing the algorithm 
    optstatus       varchar(16),    -- the algorithm status
    -- BASE OPT INFO
    rootlp          decimal,        -- IIO assings to this field the value 'nan' 
    rootime         decimal,        -- IIO assings to this field the value 'nan'
    bestbound       decimal,        -- IIO assings to this field the value 'nan'
    objvalue        decimal,        -- the objective function value
    optgap          decimal,        -- IIO assings to this field the value 'nan'
    runtime         decimal,        -- the optimization runtime in seconds
    -- INTEGER VALUES
    -- [1] nodes, [2] nodes left, [3] major iters, [4] minor iters, [5] basis chngs, [6] full rcs comp, [7] avoided loop srchs, [8] failed loop srchs, [9] succ loop srchs, [10] tempt loop srchs
    intv1           integer,        -- the number of iterations of the heurisitc algorithm computing the initial solution  
    intv2           integer,        -- IIO assings to this field the value 0
    intv3           integer,        -- the number of macro-iterarions
    intv4           integer,        -- the number of algorithm iterations
    intv5           integer,        -- the number of basis changes of the IIO Phase 2 (or simply the number of basis changes if the standard transportation simplex is applied)
    intv6           integer,        -- the number of time the algorithm computes all reduced costs (in case the shielding methods is applied to the problem solution this value has no relevant meaning)
    intv7           integer,        -- the number of loop computations skipped applying the tree-coloring (this counter is set to right value only if you compile the code with flag 'EXPTRACING_2')
    intv8           integer,        -- the number of loop computations that lead to a pivoting quantity theta NOT greater than 0 (this counter is set to right value only if you compile the code with flag 'EXPTRACING_2')
    intv9           integer,        -- the number of loop computations that lead to a pivoting quantity theta greater than 0 (this counter is set to right value only if you compile the code with flag 'EXPTRACING_2')
    intv10          integer,        -- the number of times that, in the IIO Phase 1 when tree-coloring is applied, a non-basic variable is tested for the yes/no decision of computing the related loop (this counter is set to right value only if you compile the code with flag 'EXPTRACING_2')
    -- DOUBLE VALUES
    -- [1] init sol time, [2] data init time (sorting), [3] opt time, [4] multip time, [5] rcs time, [6] basis chng time, [7] multipiv step 1 time, [8] multipiv step 2 time, [9] tot time, [10] dummy, [11] init sol impr  
    doublev1        decimal,        -- the computation time of the heuristic initial solution
    doublev2        decimal,        -- the solution of initializing problem data (e.g., the time for sorting the problem variables in the initial solution heuristic MMR is applied)
    doublev3        decimal,        -- the optimization time, i.e., the algorithm time NOT including the computation of the initial solution time and the data initialization time 
    doublev4        decimal,        -- the total time for computing problem multipliers 
    doublev5        decimal,        -- the total time for computing reduced costs
    doublev6        decimal,        -- the total time of the IIO Phase 1 and Phase 2 computation
    doublev7        decimal,        -- the total time of the IIO Phase 1 computation
    doublev8        decimal,        -- the total time of the IIO Phase 2 computation
    doublev9        decimal,        -- the solution time, i.e., the total time for computing the problem optimal solution including the initial solution time and the data initialization time
    doublev10       decimal,        -- this is a dummy value
    doublev11       decimal         -- the optimality gap of the initial solution computed as (initial_solution_objf_value - opt_objf_value) / opt_objf_value
);
