#include "colors.inc"
#include "textures.inc"
#include "woods.inc"
#include "dice.inc"
#include "stones1.inc"

//  #include "glass.inc"


global_settings {
      assumed_gamma 1.0
      max_trace_level 5
/*
      photons {
      	spacing 0.01  // higher value 'lower' quality, faster parsing.
      	autostop 0
      	jitter 0.5
      	max_trace_level 15
      }
*/
}

//background { Gray80 }

#declare Slow=true;
#declare ShowPawns=false;

camera {

	right x*1//.33333
	location <0, 0.07, 0.00>
	look_at <0.0,0.0,0>
//	location <+.25, 5.05, 0.9	>
//	look_at <+.5,0,-2>

//	location <4.5, 2.5, -3>/20
//	look_at <0.35,0.025,0>
//	angle 50
//	focal_point <0.35,0.025,0> //right dice in focus
//  	aperture 1/32//0.1
//	blur_samples 40
}

light_source {
	<-5,5,5> White
//	shadowless
#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}

light_source {
	<5,5,5> White
//	shadowless

#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}

/*
light_source {
	<-5,5,-5> White
//	shadowless
#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}

light_source {
	<5,5,-5> White
//	shadowless

#if (Slow)
	 area_light <4, 0, 0>, <0, 0, 4>, 9, 9
	adaptive 1
	jitter
#end

}
*/
//light_source { <-5,5,-5> White }


object {
	dice
	scale 0.025
	rotate 90*y
	rotate 270*x
	translate <0,0,0>
	finish {
		phong 0.9
		phong_size 60
		reflection {0.05}
		//ambient 0.5
		//diffuse 0.2
	}
}

