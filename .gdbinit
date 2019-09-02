define p_vec_size
	p *$arg0.p_size
end

define p_vec
	set $vec_head = *$arg0.p_head
	set $listSize = *$arg0.p_size
	p *$vec_head @ $listSize
end

define p_vec_at
	if $argc == 2
		set $vec_head = *$arg0.p_head
		p *($vec_head + $arg1)
	end

	if $argc == 3
		set $head = *$arg0.p_head + $arg1
		set $size = $arg2 - $arg1
		p *$head @ $size
	end
end

define p_vec2d_size_at
	set $tmp = *(*$arg0.p_head + $arg1)
	p_vec_size $tmp
end

define p_vec2d
	set $i = 0
	while $i < *$arg0.p_size
		set $tmp = *(*$arg0.p_head + $i)
		p_vec $tmp
		set $i = $i + 1
	end
end

define p_vec2d_at
	if $argc == 2
		set $tmp = *(*$arg0.p_head + $arg1)
		p_vec $tmp
	end
	
	if $argc == 3
		set $tmp = *(*$arg0.p_head + $arg1)
		p_vec_at $tmp $arg2
	end
	
	if $argc == 5
		set $i = $arg1
		while $i < $arg2
			set $tmp = *(*$arg0.p_head + $i)
			p_vec_at $tmp $arg3 $arg4
			set $i = $i + 1
		end
	end
end