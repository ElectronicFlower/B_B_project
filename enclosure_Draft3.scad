$fn=100; // use either 50 or 100 

wallthickness=1.32; // this is the wall of the lip, the box is twice as thick

internalx=33+5; // internal x dimension

internaly=41.61+5; // internal y dimension

internalz=1.5748+15; // internal overall z dimension

internal_lidz=5; // this is taken of off the internalz for the bottom height and acts as the lid internal height

lip_overlap = internal_lidz/2; // how much overlap between box and lid, this works as a default but can be shrunk as needed

lipspacing=0; // this amount is take off of the wall thickness on the lip to increase spacing, if your lid is too tight you can increase this but it will reduce the actual wall thickness on the lip so too much and you will have to increase the wallthickness to compensate. Should be 0 in ideal world, you will likely be better just lightly sanding the lip unless your printer is way out

//#############################
// INTERNAL VARIABLES, DO NOT MODIFY
internal_botz=internalz-internal_lidz;

actual_y=internaly+(4*wallthickness);
actual_x=internalx+(4*wallthickness);
actual_lidz=internal_lidz+(2*wallthickness);
actual_botz=internal_botz+(2*wallthickness);

lip_x = internalx+(2*wallthickness);
lip_y = internaly+(2*wallthickness);



module bottom() {
    union(){
    difference() {
        union() {
            scale([actual_x,actual_y,actual_botz])cube(1);
            translate([wallthickness,wallthickness,actual_botz])scale([lip_x,lip_y,lip_overlap])cube(1);
        }
        
        translate([(2*wallthickness),(2*wallthickness),(2*wallthickness)])scale([internalx,internaly,internalz])cube(1);
    }

//tabs
    tab_thickness=5;
//linear_extrude(internal_botz)
linear_extrude(tab_thickness)
translate([0,(actual_y),-10])
polygon([[0,0],[actual_x,0],[31,8],[31,8],[9,8],[9,8]]);

linear_extrude(tab_thickness)
rotate([180, 0, 0]) translate([0,0,-10])
polygon([[0,0],[actual_x,0],[31,8],[31,8],[9,8],[9,8]]);}
}


module lid() {
    difference() {
        scale([actual_x,actual_y,actual_lidz+lip_overlap])cube(1);
        
        translate([(2*wallthickness),(2*wallthickness),-.1])scale([internalx,internaly,internal_lidz+lip_overlap+.1])cube(1);
        
        translate([(wallthickness)-lipspacing/2,(wallthickness)-lipspacing/2,-.1])scale([lip_x+lipspacing,lip_y+lipspacing,lip_overlap+.3])cube(1);
    }
}

color([0,1,0]) translate([0,0,actual_botz + lip_overlap + 10])lid();


bottom();

//PCB Mock up
color([1,0,0]) translate([0 , 0, -5]) cube([33,41.61, 1.5748], center);


