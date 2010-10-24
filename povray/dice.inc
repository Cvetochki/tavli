#declare pip = sphere {
	0,.3
	pigment {
		color Black
	}
}
#declare pip_out = 1.25;
#declare pip_lateral = .475;

// create a superquadric ellipsoid shape
#declare dice =
difference {
	superellipsoid {
		<0.25, 0.25>
	}
	//1 on top
	object { pip translate <0,pip_out,0> }

	//2 on back
	object { pip translate <pip_lateral,pip_lateral,pip_out> }
	object { pip translate <-pip_lateral,-pip_lateral,pip_out> }

	//3 on left side
	object { pip translate <-pip_out,-pip_lateral,pip_lateral> }
	object { pip translate <-pip_out,0,0> }
	object { pip translate <-pip_out,pip_lateral,-pip_lateral> }

	//4 on right side
	object { pip translate <pip_out,-pip_lateral,pip_lateral> }
	object { pip translate <pip_out,-pip_lateral,-pip_lateral> }
	object { pip translate <pip_out,pip_lateral,pip_lateral> }
	object { pip translate <pip_out,pip_lateral,-pip_lateral> }

	//5 on front
	object { pip translate <pip_lateral,pip_lateral,-pip_out> }
	object { pip translate <-pip_lateral,-pip_lateral,-pip_out> }
	object { pip translate <0,0,-pip_out> }
	object { pip translate <-pip_lateral,pip_lateral,-pip_out> }
	object { pip translate <pip_lateral,-pip_lateral,-pip_out> }

	//6 on bottom
	object { pip translate <-pip_lateral,-pip_out,-pip_lateral>}
	object { pip translate <0,-pip_out,-pip_lateral>}
	object { pip translate <pip_lateral,-pip_out,-pip_lateral>}
	object { pip translate <-pip_lateral,-pip_out,pip_lateral>}
	object { pip translate <0,-pip_out,pip_lateral>}
	object { pip translate <pip_lateral,-pip_out,pip_lateral>}

	pigment {
		color <1.0,1.0,1.0>//White

	}
}
