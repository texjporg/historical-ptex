%%
%% teTeX-1.0 + tetexsupp (Web2C 7.3.2x) のとき、
%% ptex の pltotf.ch が想定している usage に修正するのに用いる。
%% ptex の pltotf.ch では、つぎの変換をする。tftopl も同様。
%% @x
%%      usage (0, PLTOTF_HELP);
%% @y
%%      usage_help (0, PLTOTF_HELP);
%% @z
%%
@x
      usage_long (0, PLTOTF_HELP, nil);
@y
      usage (0, PLTOTF_HELP);
@z
