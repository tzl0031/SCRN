load("youtube.mat")
load("SF.mat")
% sample
n = size(network, 1);

s = 10000;

ind = zeros(1, s);
for i=1:s
    ind(i) = i;
end
    

ratio = 0.30;


for i = 1
    net = network(i*ind, i*ind);
    SF_r = SF;
    IDX.training = ind(1:floor(s * ratio));
    IDX.testing = ind(ceil(s * 0.8):s);
    IDX.inference = ind(ceil(s * ratio):s);
    tic
    [pred_output, acc] = SCRN(net, IDX, group, SF_r, 20);
    toc
end
clear
