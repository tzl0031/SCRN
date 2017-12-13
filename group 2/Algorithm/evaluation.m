function [f1] = evaluation(pred_label, orig_label, method)
numClass = size(pred_label, 2);
num = size(pred_label, 1);
pred_label = full(pred_label);
orig_label = full(orig_label);
f1_k = 0;
hamming = 0;

for k =1:numClass
    denom = 2 * sum(pred_label(:, k).* orig_label(:, k));
    nom = sum(pred_label(:, k)) + sum(orig_label(:, k));

    if nom ~= 0
        f1_k = f1_k + denom / nom;
    end
end

for i = 1:num
    hamming = hamming + norm(double(xor(pred_label(i,:), orig_label(i,:))), 1);
end


macro_f1 = f1_k / k;
micro_f1 = 2* sum(sum(pred_label.*orig_label))/ (sum(sum(pred_label)) + sum(sum(orig_label)));
switch method
    case "micro"
        f1 = micro_f1;
    case "macro"
        f1 = macro_f1;
    case "hamming"
        f1 = hamming/ (numClass * num);
end
end


