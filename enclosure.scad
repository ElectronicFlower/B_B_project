//measurements are in mm 

$fn=100; // use either 50 or 100 

//PCB measurements 
PCB_x=31.8;
PCB_y=40.8;
PCB_z=1.6;
post_diameter=3;
bigger_post= post_diameter;
post_height=8.85; 

//Battery Measurements
Battery_Holder_z=15.3;
Battery_Holder_y=59;
Battery_Holder_x=32.5;

//Box Parameters
wallthickness=1.32; // this is the wall of the lip, the box is twice as thick

internalx=(50.8-5.3) ; // internal x dimension
//we want it to be 50.8mm but must subtract 4*wallthickness
//did a slight rounding up to ensure we stay under

internaly=(76.2-5.3); // internal y dimension

internalz=(38-5.5); // internal overall z dimension

internal_lidz=5; // this is taken of off the internalz for the bottom height and acts as the lid internal height

lip_overlap = internal_lidz/2 ; // how much overlap between box and lid

lipspacing=0.5; // this amount is take off of the wall thickness on the lip to increase spacing, if your lid is too tight you can increase this but it will reduce the actual wall thickness on the lip so too much and you will have to increase the wallthickness to compensate. Should be 0 in ideal world, you will likely be better just lightly sanding the lip unless your printer is way out

// INTERNAL VARIABLES, DO NOT MODIFY
internal_botz=internalz-internal_lidz;
actual_y=internaly+(4*wallthickness);
actual_x=internalx+(4*wallthickness);
actual_lidz=internal_lidz+(2*wallthickness);
actual_botz=internal_botz+(2*wallthickness);
lip_x = internalx+(2*wallthickness);
lip_y = internaly+(2*wallthickness);

//Tab Parameters 
tab_thickness=4;
tab_height=10;
screw_diameter=3.4; 
screw_x=(actual_x/2); //to translate screw hole
screw_y=(tab_height/2);

module lid() {
    difference() {
        scale([actual_x,actual_y,actual_lidz+lip_overlap])cube(1);
        
        translate([(2*wallthickness),(2*wallthickness),-.1])scale([internalx,internaly,internal_lidz+lip_overlap+.1])cube(1);
        
        translate([(wallthickness)-lipspacing/2,(wallthickness)-lipspacing/2,-.1])scale([lip_x+lipspacing,lip_y+lipspacing,lip_overlap+.3])cube(1);
    }
}
module name(){
linear_extrude(0.75)
text( "NewWave", size= 5, font="Book Antiqua:style=Bold Italic",$fn=100);
}
module boat(){
    linear_extrude(0.75) figure();
}
module figure(){
     polygon([[8,8],[16,0],[8,8],[48,8],[43,0],[16,0]]);
    polygon([[28,24],[28,8],[28,24],[28,8],[29,8],[29,24]]);
    polygon([[29,24],[29,12],[39,12],[35,17]]);
    polygon([[28,24],[28,12],[18,12]]);
    }
//vent
module vent(){
    translate([10.5,4,actual_botz + lip_overlap + 2]) cube([16.35+1, 7.35+1,actual_lidz+50]);
}
module screwHole(r1,h1,r2,h2){
    // r1 rad bottom of cone and the bolt hole, r2 rad top of cone, h1 full height of screw + cone, h2 height of cone
    // shaft first
    cylinder(r=r1,h=h1);
    // cone second
    translate([0,0,h1-h2]) cylinder(r1=r1, r2=r2, h=h2);
}
module DHT(){
    union(){
    color("white") cube([1.2,15.4,25.4]);
    color("white") translate([1.2,0,0]) cube([5.1,15.4,20.1]);
    color("silver") translate([2.7, 3.65, -6.8]) rotate(90,0,0) cube([8.1,.1,6.8]);
    }
}
module PCB(){
    union(){
        difference(){
            color("green") cube([PCB_x,PCB_y, PCB_z], center);
            
            translate([1.6 , 1.6, 0]) screwHole ((3.175/2),PCB_z,0,0);
            
            translate([1.6, 39.1, 0]) screwHole ((3.175/2),PCB_z,0, 0);
            
            translate([30.2 , 1.6, 0]) screwHole ((3.175/2),PCB_z,0,0);
            
            translate([30.2, 39.1, 0]) screwHole ((3.175/2), PCB_z, 0, 0);           
}

//area where DHT goes
   translate([10.5,2.5,0]) color("Black") cube([15.2,7.9, PCB_z+0.5], center=false);
}
}

translate([9.49-1,17.69-2,(actual_lidz+actual_botz-PCB_z)]) PCB();

rotate([0,0,-90]) translate([-28,17.69,65])DHT();

//supporting posts
module posts(x,y,z,h,r){
    translate([x,y,z]){
    cylinder(r = r, h = h);
    }
    translate([-x, y, z]){
    cylinder(r = r, h = h);
    }
    translate([-x, -y, z]){
    cylinder(r = r, h = h);
    }
    translate([x, -y, z]){
    cylinder(r =r, h = h);
    }
}
module support_posts(){
    //1 mm closer
    translate([11 , (21-1.5-1.1), 0]) difference(){
        posts(0,0,0, post_height, bigger_post);
        screwHole((screw_diameter/2), 5.9,0,0);
    }
    
    translate([37.2-1, (21-1.5), 0]) difference(){
        posts(0,0,0, post_height, bigger_post);
        screwHole((screw_diameter/2), 5.9,0,0);
    }
    //1 mm closer
        translate([37.2-1 ,53, 0]) difference(){
        posts(0,0,0, post_height, bigger_post);
        screwHole((screw_diameter/2), 5.9,0,0);
    }
    //keep
    translate([11, 53-1.1, 0]) difference(){
        posts(0,0,0, post_height, bigger_post);
        screwHole((screw_diameter/2), 5.9,0,0);
    }
}
module lid_posts(){
    color("pink") union(){
        translate([0,0,actual_botz + lip_overlap+ 5 ]) lid();
       
        translate([0,0,actual_lidz+actual_botz+.1]) support_posts();
    }
}

union(){
    rotate([0,0,180]) translate([(-actual_x-31)/2,-actual_y+2,47.5]) name();

    rotate([0,0,180]) translate([((-actual_x-54)/2),(-actual_y+10),47.5]) boat();

    difference(){
        lid_posts();
        
        translate([9.49-4.2+0.75,17.1-2,-2]) vent();
        }
}
module bottom() {
    difference() {
        union() {
            scale([actual_x,actual_y,actual_botz])cube(1);
            translate([wallthickness,wallthickness,actual_botz])scale([lip_x,lip_y,lip_overlap])cube(1);
        }
        
        translate([(2*wallthickness),(2*wallthickness),(3.82*wallthickness)])scale([internalx,internaly,internalz])cube(1);
    }
}
module tabs(){
    linear_extrude(tab_thickness)
    translate([0,(actual_y),-10])
    polygon([[0,0],[actual_x,0],[42.78,tab_height],[8,tab_height]]);

    linear_extrude(tab_thickness)
    rotate([180, 0, 0]) translate([0,0,-10])
    polygon([[0,0],[actual_x,0],[42.78,tab_height],[8,tab_height]]);
}
module BatteryHolder(){
 difference(){  
    color("black") cube(size = [Battery_Holder_x,Battery_Holder_y,Battery_Holder_z], center = false);
     
    translate([6.7,27.8,0]) screwHole((screw_diameter/2),Battery_Holder_z,0,0);
     
    translate([22.3,27.8,0]) screwHole((screw_diameter/2),Battery_Holder_z,0,0);
 }}
 
translate([8.59, 9.14,wallthickness]) BatteryHolder();
 

//Calling the bottom and tabs module and joining then cutting screw holes
color("#ADD8E6") union(){
    bottom();
    
   difference(){
        tabs();
        
        //Left Tab Screw Hole 
        //remember module screwHole(r1,h1,r2,h2) //r2 & h2 for cone (assuming we aren't using)
        translate([screw_x, -(screw_y), 0]) screwHole ((screw_diameter/2),(tab_thickness+2),0,0);
        
        // Right Tab Screw Hole
        translate([screw_x,(actual_y + screw_diameter+ 1),0]) screwHole ((screw_diameter/2),(tab_thickness+2),0,0);
    } 
}
