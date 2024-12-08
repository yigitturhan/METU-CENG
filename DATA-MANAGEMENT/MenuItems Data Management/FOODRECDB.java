package ceng.ceng351.foodrecdb;
import java.sql.*;
import java.util.ArrayList;

public class FOODRECDB implements IFOODRECDB{
    private static String user = "e2448942"; // TODO: Your userName
    private static String password = "zkf4GrVeyS_14B0H"; //  TODO: Your password
    private static String host = "momcorp.ceng.metu.edu.tr"; // host name
    private static String database = "db2448942"; // TODO: Your database name
    private static int port = 8080; // port

    private static Connection connection = null;
    @Override
    public void initialize() {
        String url = "jdbc:mysql://" + this.host + ":" + this.port + "/" + this.database + "?useSSL=false";
        try {
            Class.forName("com.mysql.cj.jdbc.Driver");
            this.connection =  DriverManager.getConnection(url, this.user, this.password);
        } catch (SQLException | ClassNotFoundException e) {
            e.printStackTrace();
        }
    }

    @Override
    public int createTables() {
        int number_of_tables_created = 0;

        String query_create_MenuItems = "CREATE TABLE IF NOT EXISTS MenuItems(" +
                "itemID INT NOT NULL, " +
                "itemName VARCHAR(40), " +
                "cuisine VARCHAR(20), " +
                "price INT, " +
                "PRIMARY KEY (itemID));";

        String query_create_Ingredients = "CREATE TABLE IF NOT EXISTS Ingredients(" +
                "ingredientID INT NOT NULL, " +
                "ingredientName VARCHAR(40), " +
                "PRIMARY KEY (ingredientID));";

        String query_create_Includes = "CREATE TABLE IF NOT EXISTS Includes(" +
                "itemID INT NOT NULL, " +
                "ingredientID INT NOT NULL, " +
                "FOREIGN KEY (itemID) REFERENCES MenuItems(itemID) " +
                "ON DELETE CASCADE, " +
                "FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID) " +
                "ON DELETE CASCADE, " +
                "PRIMARY KEY (itemID, ingredientID));";

        String query_create_Ratings = "CREATE TABLE IF NOT EXISTS Ratings(" +
                "ratingID INT NOT NULL, " +
                "itemID INT, " +
                "rating INT, " +
                "ratingDate DATE, " +
                "FOREIGN KEY (itemID) REFERENCES MenuItems(itemID) ON DELETE CASCADE, " +
                "PRIMARY KEY (ratingID));";
        String query_create_DietaryCategories = "CREATE TABLE IF NOT EXISTS DietaryCategories(" +
                "ingredientID INT NOT NULL, " +
                "dietaryCategory VARCHAR(20) NOT NULL, " +
                "FOREIGN KEY (ingredientID) REFERENCES Ingredients(ingredientID) " +
                "ON DELETE CASCADE, " +
                "PRIMARY KEY (ingredientID, dietaryCategory));";


        try{
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_create_MenuItems);
            number_of_tables_created++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        try{
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_create_Ingredients);
            number_of_tables_created++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        try{
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_create_Includes);
            number_of_tables_created++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        try{
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_create_Ratings);
            number_of_tables_created++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        try{
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_create_DietaryCategories);
            number_of_tables_created++;
            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        return number_of_tables_created;
    }

    @Override
    public int dropTables() {
        int number_of_tables_dropped = 0;



        String query_drop_MenuItems = "DROP TABLE IF EXISTS MenuItems;";
        String query_drop_Ingredients = "DROP TABLE IF EXISTS Ingredients;";
        String query_drop_Includes = "DROP TABLE IF EXISTS Includes;";
        String query_drop_Ratings = "DROP TABLE IF EXISTS Ratings;";
        String query_drop_DietaryCategories = "DROP TABLE IF EXISTS DietaryCategories;";


        try {
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_drop_DietaryCategories);
            number_of_tables_dropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_drop_Ratings);
            number_of_tables_dropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_drop_Includes);
            number_of_tables_dropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_drop_Ingredients);
            number_of_tables_dropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }

        try {
            Statement statement = this.connection.createStatement();
            statement.executeUpdate(query_drop_MenuItems);
            number_of_tables_dropped++;
            statement.close();
        } catch (SQLException e) {
            e.printStackTrace();
        }
        return number_of_tables_dropped;
    }

    @Override
    public int insertMenuItems(MenuItem[] items) {
        int numberofRowsInserted = 0;


        for (int i = 0; i < items.length; i++)
        {
            try {
                MenuItem menu = items[i];

                PreparedStatement stmt=this.connection.prepareStatement("insert into MenuItems values(?,?,?,?)");
                stmt.setInt(1,menu.getItemID());
                stmt.setString(2,menu.getItemName());
                stmt.setString(3,menu.getCuisine());
                stmt.setInt(4,menu.getPrice());
                stmt.executeUpdate();
                stmt.close();
                numberofRowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return numberofRowsInserted;
    }

    @Override
    public int insertIngredients(Ingredient[] ingredients) {
        int numberofRowsInserted = 0;


        for (int i = 0; i < ingredients.length; i++)
        {
            try {
                Ingredient ing = ingredients[i];

                PreparedStatement stmt=this.connection.prepareStatement("insert into Ingredients values(?,?)");
                stmt.setInt(1,ing.getIngredientID());
                stmt.setString(2,ing.getIngredientName());
                stmt.executeUpdate();
                stmt.close();
                numberofRowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return numberofRowsInserted;
    }

    @Override
    public int insertIncludes(Includes[] includes) {
        int numberofRowsInserted = 0;


        for (int i = 0; i < includes.length; i++)
        {
            try {
                Includes inc = includes[i];

                PreparedStatement stmt=this.connection.prepareStatement("insert into Includes values(?,?)");
                stmt.setInt(1,inc.getItemID());
                stmt.setInt(2,inc.getIngredientID());
                stmt.executeUpdate();
                stmt.close();
                numberofRowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return numberofRowsInserted;
    }

    @Override
    public int insertDietaryCategories(DietaryCategory[] categories) {
        int numberofRowsInserted = 0;


        for (int i = 0; i < categories.length; i++)
        {
            try {
                DietaryCategory cat = categories[i];

                PreparedStatement stmt=this.connection.prepareStatement("insert into DietaryCategories values(?,?)");
                stmt.setInt(1,cat.getIngredientID());
                stmt.setString(2,cat.getDietaryCategory());
                stmt.executeUpdate();
                stmt.close();
                numberofRowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return numberofRowsInserted;
    }

    @Override
    public int insertRatings(Rating[] ratings) {
        int numberofRowsInserted = 0;


        for (int i = 0; i < ratings.length; i++)
        {
            try {
                Rating rat = ratings[i];

                PreparedStatement stmt=this.connection.prepareStatement("insert into Ratings values(?,?,?,?)");
                stmt.setInt(1,rat.getRatingID());
                stmt.setInt(2,rat.getItemID());
                stmt.setInt(3,rat.getRating());
                stmt.setString(4,rat.getRatingDate());
                stmt.executeUpdate();
                stmt.close();
                numberofRowsInserted++;

            }
            catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return numberofRowsInserted;
    }

    @Override
    public MenuItem[] getMenuItemsWithGivenIngredient(String name) {

        ArrayList<MenuItem> temporaryArray = new ArrayList<MenuItem>();
        ResultSet res;

        String get_query = "SELECT DISTINCT m.itemID, m.itemName, m.cuisine, m.price " +
                "FROM MenuItems m, Ingredients i, Includes n " +
                "WHERE i.ingredientID = n.ingredientID  AND  n.itemID = m.itemID AND i.ingredientName = '"  + name +
                "' ORDER BY m.itemID ASC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int item_id = res.getInt("itemID");
                String item_name = res.getString("itemName");
                String cuisine = res.getString("cuisine") ;
                int price = res.getInt("price");
                temporaryArray.add(new MenuItem(
                        item_id, item_name, cuisine, price));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        MenuItem[] ret_list =
                new MenuItem[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }

        return ret_list;
    }

    @Override
    public MenuItem[] getMenuItemsWithoutAnyIngredient() {
        ArrayList<MenuItem> temporaryArray = new ArrayList<>();
        ResultSet res;

        String get_query = "SELECT DISTINCT m.itemID, m.itemName, m.cuisine, m.price " +
                "FROM MenuItems m " +
                "WHERE NOT EXISTS(SELECT DISTINCT m.itemID " +
                "FROM Includes n1 " +
                "WHERE m.itemID = n1.itemID)" +
                "ORDER BY m.itemID ASC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int item_id = res.getInt("itemID");
                String item_name = res.getString("itemName");
                String cuisine = res.getString("cuisine") ;
                int price = res.getInt("price");
                temporaryArray.add(new MenuItem(
                        item_id, item_name, cuisine, price));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        MenuItem[] ret_list =
                new MenuItem[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }

        return ret_list;
    }

    @Override
    public Ingredient[] getNotIncludedIngredients() {
        ArrayList<Ingredient> temporaryArray = new ArrayList<>();
        ResultSet res;

        String get_query = "SELECT DISTINCT ing.ingredientID, ing.ingredientName " +
                "FROM Ingredients ing " +
                "WHERE ing.ingredientID NOT IN(SELECT DISTINCT ing2.ingredientID " +
                "FROM Ingredients ing2, Includes n " +
                "WHERE n.ingredientID = ing2.ingredientID)" +
                "ORDER BY ing.ingredientID ASC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int ing_id = res.getInt("ingredientID");
                String ing_name = res.getString("ingredientName");
                temporaryArray.add(new Ingredient(
                        ing_id, ing_name));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        Ingredient[] ret_list =
                new Ingredient[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }

        return ret_list;
    }

    @Override
    public MenuItem getMenuItemWithMostIngredients() {
        ArrayList<MenuItem> temporaryArray = new ArrayList<>();
        ResultSet res;

        String get_query = "SELECT m.itemID, m.itemName, m.cuisine, m.price "+
                "FROM MenuItems m , Includes n WHERE m.itemID = n.itemID "+
                "GROUP BY itemID "+
                "ORDER BY COUNT(m.itemID) DESC "+
                "LIMIT 0,1 ;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int item_id = res.getInt("itemID");
                String item_name = res.getString("itemName");
                String cuisine = res.getString("cuisine") ;
                int price = res.getInt("price");
                temporaryArray.add(new MenuItem(
                        item_id, item_name, cuisine, price));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        MenuItem[] ret_list =
                new MenuItem[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }
        if(temporaryArray.size() == 0){
            return null;
        }

        return ret_list[0];
    }

    @Override
    public QueryResult.MenuItemAverageRatingResult[] getMenuItemsWithAvgRatings() {
        ArrayList<QueryResult.MenuItemAverageRatingResult> tmp_array = new ArrayList<>();
        ResultSet res;
        String get_query = "(SELECT DISTINCT m.itemID, m.itemName, AVG(r.rating) AS ave "+
                "FROM MenuItems m, Ratings r "+
                "WHERE m.itemID = r.itemID GROUP BY m.itemID, m.itemName) " +
                "UNION "+
                "(SELECT DISTINCT m.itemID, m.itemName, NULL AS ave "+
                "FROM MenuItems m "+
                "WHERE m.itemID NOT IN (SELECT DISTINCT m1.itemID FROM MenuItems m1, Ratings r1 "+
                "WHERE m1.itemID = r1.itemID)) ORDER BY AVE DESC; ";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);


            while(res.next()){
                String it_id = res.getString("itemID");
                String it_name = res.getString("itemName");
                String ave = res.getString("ave") ;
                tmp_array.add(new QueryResult.MenuItemAverageRatingResult(
                        it_id, it_name, ave));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        QueryResult.MenuItemAverageRatingResult[] user_array =
                new QueryResult.MenuItemAverageRatingResult[tmp_array.size()];

        for(int i = 0; i < tmp_array.size(); i++){
            user_array[i] = tmp_array.get(i);
        }

        return user_array;
    }

    @Override
    public MenuItem[] getMenuItemsForDietaryCategory(String category) {
        ArrayList<MenuItem> temporaryArray = new ArrayList<>();
        ResultSet res;

        String get_query = "SELECT DISTINCT m.itemID, m.itemName, m.cuisine, m.price "+
                "FROM MenuItems m , Includes n , DietaryCategories d "+
                "WHERE m.itemID = n.itemID AND d.ingredientID = n.ingredientID AND d.dietaryCategory = '"+ category +
                "' GROUP BY m.itemID "+
                "HAVING COUNT(m.itemID) = (SELECT COUNT(*) FROM Includes n1 WHERE m.itemID = n1.itemID) "+
                "ORDER BY m.itemID ASC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int item_id = res.getInt("itemID");
                String item_name = res.getString("itemName");
                String cuisine = res.getString("cuisine") ;
                int price = res.getInt("price");
                temporaryArray.add(new MenuItem(
                        item_id, item_name, cuisine, price));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        MenuItem[] ret_list =
                new MenuItem[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }
        return ret_list;
    }

    @Override
    public Ingredient getMostUsedIngredient() {
        ArrayList<Ingredient> temporaryArray = new ArrayList<>();
        ResultSet res;

        String get_query = "SELECT DISTINCT ing.ingredientID, ing.ingredientName " +
                "FROM Ingredients ing , MenuItems m, Includes n " +
                "WHERE ing.ingredientID = n.ingredientID AND n.itemID = m.itemID "+
                "GROUP BY ing.ingredientID "+
                "ORDER BY COUNT(ing.ingredientID) DESC "+
                "LIMIT 0,1 ;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int ing_id = res.getInt("ingredientID");
                String ing_name = res.getString("ingredientName");
                temporaryArray.add(new Ingredient(
                        ing_id, ing_name));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        Ingredient[] ret_list =
                new Ingredient[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }
        if(temporaryArray.size() == 0){
            return null;
        }

        return ret_list[0];
    }

    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgRating() {
        ArrayList<QueryResult.CuisineWithAverageResult> tmp_array = new ArrayList<>();
        ResultSet res;
        String get_query = "(SELECT DISTINCT m.cuisine, AVG(r.rating) as ave "+
                "FROM MenuItems m, Ratings r "+
                "WHERE m.itemID = r.itemID AND r.rating > 0 "+
                "GROUP BY m.cuisine) "+
                "UNION "+
                "(SELECT DISTINCT m.cuisine, NULL as ave "+
                "FROM MenuItems m "+
                "WHERE m.cuisine NOT IN "+
                "(SELECT DISTINCT m1.cuisine FROM MenuItems m1, Ratings r1 WHERE m1.itemID = r1.itemID)) "+
                "ORDER BY ave DESC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);


            while(res.next()){
                String av = res.getString("ave");
                String cus = res.getString("cuisine") ;
                tmp_array.add(new QueryResult.CuisineWithAverageResult(
                        cus, av));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        QueryResult.CuisineWithAverageResult[] user_array =
                new QueryResult.CuisineWithAverageResult[tmp_array.size()];

        for(int i = 0; i < tmp_array.size(); i++){
            user_array[i] = tmp_array.get(i);
        }

        return user_array;
    }

    @Override
    public QueryResult.CuisineWithAverageResult[] getCuisinesWithAvgIngredientCount() {
        ArrayList<QueryResult.CuisineWithAverageResult> tmp_array = new ArrayList<>();
        ResultSet res;
        String get_query = "SELECT t1.cuisine, t1.c1*1.0 / t2.c2 as ave "+
        "FROM (SELECT m1.cuisine, 0 as c1 "+
                "FROM MenuItems m1 "+
                "WHERE m1.cuisine NOT IN (SELECT m.cuisine "+
                        "FROM MenuItems m, Ingredients i , Includes n "+
                        "WHERE i.ingredientID = n.ingredientID AND m.itemID = n.itemID "+
                        "GROUP BY m.cuisine) "+
                "UNION "+

                "SELECT m.cuisine , COUNT(m.cuisine) as c1 "+
                "FROM MenuItems m, Ingredients i , Includes n "+
                "WHERE i.ingredientID = n.ingredientID AND m.itemID = n.itemID "+
                "GROUP BY m.cuisine) as t1 , (SELECT m.cuisine, COUNT(m.cuisine) as c2 "+
        "FROM MenuItems m "+
        "GROUP BY m.cuisine) as t2 "+
        "WHERE t1.cuisine = t2.cuisine "+
        "ORDER BY ave DESC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);


            while(res.next()){
                String av = res.getString("ave");
                String cus = res.getString("cuisine") ;
                tmp_array.add(new QueryResult.CuisineWithAverageResult(
                        cus, av));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }

        QueryResult.CuisineWithAverageResult[] user_array =
                new QueryResult.CuisineWithAverageResult[tmp_array.size()];

        for(int i = 0; i < tmp_array.size(); i++){
            user_array[i] = tmp_array.get(i);
        }

        return user_array;

    }

    @Override
    public int increasePrice(String ingredientName, String increaseAmount) {
        int numberofRowsupdated= 0;
        ArrayList<MenuItem> temporaryArray = new ArrayList<>();
        int amo = Integer.parseInt(increaseAmount);


        ResultSet res;



        String get_query = "SELECT DISTINCT m.itemID, m.itemName, m.cuisine, m.price " +
                "FROM Ingredients ing , MenuItems m, Includes n " +
                "WHERE ing.ingredientID = n.ingredientID AND n.itemID = m.itemID AND ing.ingredientName = '"+ ingredientName +
                "' ORDER BY m.itemID DESC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int item_id = res.getInt("itemID");
                String item_name = res.getString("itemName");
                String cuisine = res.getString("cuisine");
                int price = res.getInt("price");
                temporaryArray.add(new MenuItem(
                        item_id, item_name, cuisine, price + amo));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }
        MenuItem[] ret_list =
                new MenuItem[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }

        for(int i=0; i<temporaryArray.size();i++){
            numberofRowsupdated++;
            int x = ret_list[i].getPrice();
            String np = Integer.toString(x);
            int y = ret_list[i].getItemID();
            String nid = Integer.toString(y);
            String exec_query = "UPDATE MenuItems SET price = " + np +
                    " WHERE itemID = " + nid + " ;";
            try{
                Statement statement = this.connection.createStatement();
                statement.executeUpdate(exec_query);
                statement.close();
            }catch (SQLException e) {
                e.printStackTrace();
            }
        }

        return numberofRowsupdated;
    }

    @Override
    public Rating[] deleteOlderRatings(String date) {
        ArrayList<Rating> temporaryArray = new ArrayList<>();
        ResultSet res;

        String get_query = "SELECT DISTINCT r.ratingID, r.itemID, r.rating, r.ratingDate "+
                "FROM Ratings r "+
                "WHERE r.ratingDate < '"+ date + "' ORDER BY r.ratingID ASC;";

        try{
            Statement statement = this.connection.createStatement();
            res = statement.executeQuery(get_query);

            while(res.next()){
                int rat_id = res.getInt("ratingID");
                int it_id = res.getInt("itemID");
                int rat = res.getInt("rating");
                String dat = res.getString("ratingDate");


                temporaryArray.add(new Rating(
                        rat_id, it_id, rat, dat));
            }

            statement.close();
        }catch (SQLException e) {
            e.printStackTrace();
        }


        Rating[] ret_list =
                new Rating[temporaryArray.size()];

        for(int i = 0; i < temporaryArray.size(); i++){
            ret_list[i] = temporaryArray.get(i);
        }
        for(int i=0; i<temporaryArray.size();i++){
            int rat_id = ret_list[i].getRatingID();
            String s = Integer.toString(rat_id);
            String exec_query = "DELETE FROM Ratings WHERE ratingID = "+ s + " ;";
            try{
                Statement statement = this.connection.createStatement();
                statement.executeUpdate(exec_query);
                statement.close();
            }catch (SQLException e) {
                e.printStackTrace();
            }
        }
        return ret_list;
    }
}
