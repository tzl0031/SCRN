% load("youtube.mat")
% SF = edgeclustering(network, 1000);
% sample
n = size(network, 1);

s = 100000;
% r = randi(n, 1, s);
ind = zeros(1, s);
for i=1:s
    ind(i) = i;
end    

net = network(ind, ind);
SF_r = SF(ind,:);


IDX.training = ind(1:floor(s * 0.09));
IDX.testing = ind(ceil(s * 0.8):s);
IDX.inference = ind(ceil(s * 0.09):s);
tic

[pred_label, pred_output] = SCRN_RL(net, IDX,group, SF_r,20);
toc
