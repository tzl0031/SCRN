function [pred_label,accuracy]= SCRN(Net,idx,Label,SF,maxiter)
num_node = size(Net,1);
train_idx = idx.training;
inference_idx = idx.inference;
test_idx = idx.testing;
train_label = Label(train_idx,:);
test_label = Label(test_idx,:);
inference_label = Label(inference_idx,:);
SF_train = SF(train_idx,:);
SF_inference = SF(inference_idx,:);

num_feature = size(SF_train, 2); 
num_class = size(train_label,2); 
num_inference = size(inference_label,1);


class_feature = zeros(num_class,num_feature);
for i=1:num_class
    member = (train_label(:,i)==1);
    class_feature(i,:) = sum(SF_train(member,:),1);
end

% class propagation probability
prob_cp = class_propagation(SF_inference, class_feature);

iter = 1;
output = sparse(num_node,num_class); 
output(train_idx,:) = train_label;
old_label = sparse(num_inference,num_class);
pred_label = sparse(num_inference,num_class);
changed = num_inference; 
belta = 0.5;
alpha = 0.99;
P_old = sparse(num_inference,num_class);
while(iter <= maxiter && changed >= 0) 
    P = zeros(num_inference,num_class);
    for i = 1:num_inference
        tmp = find(Net(inference_idx(i),:)>0); 
        Sim = node_weight(Net,inference_idx(i),tmp); 
        P(i,:) = Sim*(repmat(prob_cp(i,:),length(tmp),1).* output(tmp,:));
        if(sum(P(i,:)))
            P(i,:) = P(i,:)/sum(P(i,:));
        end
        clear tmp;
    end 

    P_update = belta*P + (1-belta)*P_old;
    tp = inference_idx(find(sum(P_update,2)));
    tp2 = (sum(P_update,2)>0);
    P_update(tp2,:) = P_update(tp2,:)./repmat(sum(P_update(tp2,:),2),1,num_class);
    
    [output(tp,:),pred_label(tp2,:)] = prediction(P_update(tp2,:),Label(tp,:));

    changed = full(sum(sum(xor(pred_label,old_label))));
    fprintf('Iteration %d, %d prediction changed.\n',iter,changed);
   

    class_feature = zeros(num_class,num_feature);
    for i = 1:num_class
        member = (output(:,i)>0);
        class_feature(i,:) = output(member,i)'*SF(member,:);
        class_feature(i,:) = class_feature(i,:)./length(member);
    end
    %% update class propagation probability
    prob_cp = class_propagation(SF_inference,class_feature);
    old_label = pred_label;
    iter = iter +1;
    P_old = P_update;
    belta = belta*alpha;
end
%% Evaluation 
[C,I1] = intersect(inference_idx,test_idx);
pred_label = pred_label(I1,:);
accuracy(1) = evaluation(pred_label,test_label, "micro");
accuracy(2) = evaluation(pred_label,test_label, "macro");
accuracy(3) = evaluation(pred_label,test_label, "hamming");
fprintf('Iteration %d, micro f1 is %f : \n', iter, accuracy(1));
fprintf('Iteration %d, macro f1 is %f : \n', iter, accuracy(2));
fprintf('Iteration %d, hamming loss is %f : \n', iter, accuracy(3));
end





