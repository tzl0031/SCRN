function [output,pred_label] = prediction(P,test_label)
[num_test,numClass] = size(P);
output = zeros(num_test,numClass);
pred_label = zeros(num_test,numClass);
N = sum(test_label,2); 
for i=1:num_test
    [v,idx]= sort(full(P(i,:)),2,'descend');
    pred_label(i,idx(1:N(i))) = ones(1,N(i));
    output(i,:) = P(i,:)./sum(P(i,:)); 
end
end
