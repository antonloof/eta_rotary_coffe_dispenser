clc, close all, clear
% linearity measure 10 cups is 1201 thus 1 cup is about 120
weights = [16 160 426 813 1320 1887 2642 3508 4496 5601];
weights_zero_delay =         [17 149 404 509 1002 1618 2244 3096 4071 4292];
weights_negative_200_delay = [16 137 380 746 1233 1838 2562 3408 4368 4643];
weights_negative_150_delay = [14 135 382 446 934  1542 2264 2322 3287 4379];

lin = diff(weights_negative_150_delay);
lin_x = [1 2 4 5 6 8 9];
lin = lin(lin_x);

a = polyfit(lin_x, lin, 1);

hold on
plot(lin_x, lin, 'o');
plot(0:10, a(1)*(0:10)+a(2))


%%
clc, close all, clear 
repeated_7 = [3 971 1462 2399 2811 3746 4713 5680];
repeated_7 = [3 968 1925 2870];
repeated_7 = [3 589 ];
plot(diff(repeated_7))
