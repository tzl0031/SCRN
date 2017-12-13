function [community_indicator] = edgeclustering(network, k)
    % convert network data into edge-centric view
    [indexI, indexJ] = find(tril(network));
    m = length(indexI); % number of edges
    n = size(network, 1);  % number of nodes
    clear network; % clear network to save memory
  
    disp('generating edge data ....');
    A = sparse(indexI, 1:m, 1, n, m);
    A = A | sparse(indexJ, 1:m, 1, n, m);
    edgedata = double(A);
    clear A; 
        
    % normalize the network data into unit length
    % each edge has two nodes, thus normalized by 1/sqrt(2)
    edgedata = 1/sqrt(2) * edgedata; 
            
    disp('running k-means on edge data...');
    % run k-means on edge data
    [idx] = scalable_kmeans(edgedata, k);
    idx = idx+1;  % since the returned idx starts from 0, we increase it by 1 for later convenience.
    clear edgedata;
   
    disp('convert edgeclustering result into node clusters...');
    % 'construct feature indicator matrix'
    community_indicator = sign(sparse(indexI, idx, 1, n, k));
    B = sign(sparse(indexJ, idx, 1, n, k));
    community_indicator  = double(community_indicator | B);  
    clear B;
    
    disp('normalize rows sum to 1...');
    inverse_row_sum = 1./sum(community_indicator,2);
    community_indicator = sparse(1:n, 1:n, inverse_row_sum, n, n) * community_indicator;

    
    disp('edgeclustering finished');
    
    
    

   