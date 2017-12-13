function Sim = class_propagation(testing,Class_feature)
[num_test,numf] = size(testing);
numClass = size(Class_feature,1);
Sim = zeros(num_test,numClass);
A = zeros(num_test,numf);
B = zeros(numClass,numf);
belta = 0.01; 
for i = 1:numClass
    tmp = sum(min(testing.^belta,repmat(Class_feature(i,:).^belta,num_test,1)),2);
    Sim(:,i) = tmp';
end
tp = find(sum(Sim,2)>0);
Sim(tp,:) = Sim(tp,:)./repmat(sum(Sim(tp,:),2),1,numClass);        
end