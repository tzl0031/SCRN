function [pred_label,accuracy]= SCRN_RL(Net,IDX,Label,SF,maxiter)
numNodes = size(Net,1);
tr_IDX = IDX.training;
inference_IDX = IDX.inference;
test_IDX = IDX.testing;
tr_label = Label(tr_IDX,:);
test_label = Label(test_IDX,:);
inference_label = Label(inference_IDX,:);
SF_tr = SF(tr_IDX,:);
SF_inference = SF(inference_IDX,:);

[numtr,numf] = size(SF_tr); 
numClass = size(tr_label,2); 
num_inference = size(inference_label,1);


Class_feature = zeros(numClass,numf);
for i=1:numClass
    member = (tr_label(:,i)==1);
    Class_feature(i,:) = sum(SF_tr(member,:),1);
end

% class propagation probability
Prob_cp = class_propagation(SF_inference, Class_feature);

% Algorithm begin
iter = 1;
output = sparse(numNodes,numClass); 
output(tr_IDX,:) = tr_label;
old_label = sparse(num_inference,numClass);
pred_label = sparse(num_inference,numClass);
changed = num_inference; 
belta = 0.5;
alpha = 0.99;
P_total_old = sparse(num_inference,numClass);
while(iter <= maxiter && changed >= 0) 
    P = zeros(num_inference,numClass);
    for i = 1:num_inference
        tmp = find(Net(inference_IDX(i),:)>0); 
        Sim = node_weight(Net,inference_IDX(i),tmp); 
        P(i,:) = Sim*(repmat(Prob_cp(i,:),length(tmp),1).* output(tmp,:));
        if(sum(P(i,:)))
            P(i,:) = P(i,:)/sum(P(i,:));
        end
        clear tmp;
    end 

    P_total_new = belta*P + (1-belta)*P_total_old;
    tp = inference_IDX(find(sum(P_total_new,2)));
    tp2 = (sum(P_total_new,2)>0);
    P_total_new(tp2,:) = P_total_new(tp2,:)./repmat(sum(P_total_new(tp2,:),2),1,numClass);
    
    [output(tp,:),pred_label(tp2,:)] = prediction(P_total_new(tp2,:),Label(tp,:));

    changed = full(sum(sum(xor(pred_label,old_label))));
    fprintf('Iteration %d, %d prediction changed.\n',iter,changed);
   

    Class_feature = zeros(numClass,numf);
    for i = 1:numClass
        member = (output(:,i)>0);
        Class_feature(i,:) = output(member,i)'*SF(member,:);
        Class_feature(i,:) = Class_feature(i,:)./length(member);
    end
    %% update class propagation probability
    Prob_cp = class_propagation(SF_inference,Class_feature);
    old_label = pred_label;
    iter = iter +1;
    P_total_old = P_total_new;
    belta = belta*alpha;
end
%% Evaluation 
[C,I1] = intersect(inference_IDX,test_IDX);
pred_label = pred_label(I1,:);
accuracy(1) = evaluation(pred_label,test_label, "micro");
accuracy(2) = evaluation(pred_label,test_label, "macro");
fprintf('Iteration %d, micro f1 is %f : \n', iter, accuracy(1));
fprintf('Iteration %d, macro f1 is %f : \n', iter, accuracy(2));
end





