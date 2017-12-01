function Sim = node_weight(Net,IDX1,IDX2)
Sim = Net(IDX1,IDX2)/sum(Net(IDX1,IDX2));
end
