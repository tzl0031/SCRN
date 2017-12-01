function [f1] = evaluation(pred_label, orig_label, method)
numClass = size(pred_label, 2);
pred_label = full(pred_label);
orig_label = full(orig_label);
f1_k = 0;

for k =1:numClass
    denom = 2 * sum(pred_label(:, k).* orig_label(:, k));
    nom = sum(pred_label(:, k)) + sum(orig_label(:, k));
    if nom ~= 0
        f1_k = f1_k + denom / nom;
    end
end


macro_f1 = f1_k / k;
micro_f1 = 2* sum(sum(pred_label.*orig_label))/ (sum(sum(pred_label)) + sum(sum(orig_label)));
if method == "micro"
    f1 = micro_f1;
else
    f1 = macro_f1;
end


